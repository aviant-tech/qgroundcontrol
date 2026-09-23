/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ScheduledFlightsManager.h"
#include "AviantMissionTools.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QGC_LOGGING_CATEGORY(ScheduledFlightsManagerLog, "ScheduledFlightsManagerLog")

static constexpr int    kPollIntervalMs     = 30 * 1000;
static constexpr int    kRequestTimeoutMs   = 20 * 1000;
static constexpr double kDashMeters         = 100;
static constexpr double kGapMeters          = 100;
static constexpr double kRetraceToleranceMeters = 1;

ScheduledFlightsManager::ScheduledFlightsManager(QObject* parent)
    : QObject(parent)
{
    AviantSettings* aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();
    connect(aviantSettings->showScheduledFlights(),             &Fact::rawValueChanged, this, &ScheduledFlightsManager::_refresh);
    connect(aviantSettings->missionToolsUrl(),                  &Fact::rawValueChanged, this, &ScheduledFlightsManager::_missionToolsUrlChanged);
    connect(aviantSettings->scheduledFlightsLookaheadMinutes(), &Fact::rawValueChanged, this, &ScheduledFlightsManager::_updatePaths);

    connect(&_pollTimer, &QTimer::timeout, this, &ScheduledFlightsManager::_refresh);
    _pollTimer.start(kPollIntervalMs);
    _refresh();
}

ScheduledFlightsManager::FlightState ScheduledFlightsManager::classify(const QDateTime& start, const QDateTime& end, const QDateTime& now, int lookaheadMinutes)
{
    if (now >= end) {
        return Hidden;
    }
    if (now >= start) {
        return Open;
    }
    return now.secsTo(start) <= lookaheadMinutes * 60 ? Soon : Hidden;
}

QList<QGeoCoordinate> ScheduledFlightsManager::trimReturnTrack(const QList<QGeoCoordinate>& path)
{
    int turnIndex = 0;
    double maxDistance = 0;
    for (int i = 1; i < path.count(); i++) {
        double distance = path.first().distanceTo(path[i]);
        if (distance > maxDistance) {
            maxDistance = distance;
            turnIndex = i;
        }
    }

    for (int i = turnIndex + 1; i < path.count(); i++) {
        bool retraced = false;
        for (int j = 0; j <= turnIndex && !retraced; j++) {
            retraced = path[i].distanceTo(path[j]) <= kRetraceToleranceMeters;
        }
        if (!retraced) {
            return path;
        }
    }

    return path.mid(0, turnIndex + 1);
}

QList<QVariantList> ScheduledFlightsManager::dashPath(const QList<QGeoCoordinate>& path, double dashMeters, double gapMeters)
{
    Q_ASSERT(dashMeters > 0 && gapMeters > 0);

    QList<QVariantList> dashes;
    if (path.isEmpty()) {
        return dashes;
    }

    QVariantList dash = { QVariant::fromValue(path.first()) };
    bool   drawing   = true;
    double remaining = dashMeters;  // Distance left of the current dash or gap

    for (int i = 1; i < path.count(); i++) {
        QGeoCoordinate from = path[i - 1];
        const QGeoCoordinate& to = path[i];
        double segmentLength = from.distanceTo(to);
        if (segmentLength == 0) {
            continue;
        }

        while (segmentLength > remaining) {
            from = from.atDistanceAndAzimuth(remaining, from.azimuthTo(to));
            segmentLength -= remaining;
            dash.append(QVariant::fromValue(from));
            if (drawing) {
                dashes.append(dash);
            }
            dash = { QVariant::fromValue(from) };
            drawing = !drawing;
            remaining = drawing ? dashMeters : gapMeters;
        }
        remaining -= segmentLength;
        if (drawing) {
            dash.append(QVariant::fromValue(to));
        }
    }

    if (drawing && dash.count() >= 2) {
        dashes.append(dash);
    }
    return dashes;
}

void ScheduledFlightsManager::_refresh(void)
{
    _updatePaths();

    AviantSettings* aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();
    QString baseUrl = aviantSettings->missionToolsUrl()->rawValue().toString();
    if (_reply || !aviantSettings->showScheduledFlights()->rawValue().toBool() || baseUrl.isEmpty()) {
        return;
    }

    QNetworkRequest request = AviantMissionTools::createMmsRequest(AviantMissionTools::getMmsUrl(AviantMissionTools::FetchScheduledFlights, baseUrl));
    request.setTransferTimeout(kRequestTimeoutMs);
    QNetworkReply* reply = _networkAccessManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { _requestComplete(reply); });
    _reply = reply;
}

void ScheduledFlightsManager::_missionToolsUrlChanged(void)
{
    // Flights from the previous server are no longer relevant
    if (_reply) {
        QNetworkReply* reply = _reply;
        _reply = nullptr;
        reply->abort();
    }
    _flights.clear();
    _refresh();
}

void ScheduledFlightsManager::_requestComplete(QNetworkReply* reply)
{
    reply->deleteLater();
    if (reply != _reply) {
        // Aborted request
        return;
    }
    _reply = nullptr;

    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(ScheduledFlightsManagerLog) << "Fetching scheduled flights failed:" << reply->errorString();
        return;
    }

    _parseResponse(reply->readAll());
    _updatePaths();
}

void ScheduledFlightsManager::_parseResponse(const QByteArray& bytes)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bytes, &parseError);
    if (!jsonDoc.isArray()) {
        qCWarning(ScheduledFlightsManagerLog) << "Scheduled flights response is not a JSON array:" << parseError.errorString();
        return;
    }

    _flights.clear();
    for (const QJsonValue& value : jsonDoc.array()) {
        const QJsonObject json = value.toObject();
        if (json["cancelled"].isString()) {
            continue;
        }

        Flight flight;
        flight.start = QDateTime::fromString(json["flight_window_start"].toString(), Qt::ISODate);
        flight.end = QDateTime::fromString(json["flight_window_end"].toString(), Qt::ISODate);
        for (const QJsonValue& point : json["flight_path"].toArray()) {
            const QJsonArray lngLat = point.toArray();
            flight.path.append(QGeoCoordinate(lngLat[1].toDouble(), lngLat[0].toDouble()));
        }

        if (!flight.start.isValid() || !flight.end.isValid() || flight.path.count() < 2) {
            qCWarning(ScheduledFlightsManagerLog) << "Skipping invalid scheduled flight" << json["reference"].toString();
            continue;
        }
        flight.path = trimReturnTrack(flight.path);
        _flights.append(flight);
    }
}

void ScheduledFlightsManager::_updatePaths(void)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    int lookaheadMinutes = qgcApp()->toolbox()->settingsManager()->aviantSettings()->scheduledFlightsLookaheadMinutes()->rawValue().toInt();

    QList<QPair<FlightState, QList<QGeoCoordinate>>> visibleFlights;
    for (const Flight& flight : _flights) {
        FlightState state = classify(flight.start, flight.end, now, lookaheadMinutes);
        if (state != Hidden) {
            visibleFlights.append(qMakePair(state, flight.path));
        }
    }

    // MapItemView recreates all delegates when the model changes, so only emit on actual changes
    if (visibleFlights == _visibleFlights) {
        return;
    }
    _visibleFlights = visibleFlights;

    _paths.clear();
    for (const auto& visibleFlight : _visibleFlights) {
        if (visibleFlight.first == Open) {
            QVariantList path;
            for (const QGeoCoordinate& coordinate : visibleFlight.second) {
                path.append(QVariant::fromValue(coordinate));
            }
            _paths.append(QVariant(path));
        } else {
            for (const QVariantList& dash : dashPath(visibleFlight.second, kDashMeters, kGapMeters)) {
                _paths.append(QVariant(dash));
            }
        }
    }
    emit pathsChanged();
}
