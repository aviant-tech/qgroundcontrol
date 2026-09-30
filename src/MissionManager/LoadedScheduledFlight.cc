/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "LoadedScheduledFlight.h"
#include "AviantMissionTools.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

QGC_LOGGING_CATEGORY(LoadedScheduledFlightLog, "LoadedScheduledFlightLog")

static constexpr int kHeartbeatMs       = 30 * 1000;    // MMS drops claims not renewed within 90 s
static constexpr int kRequestTimeoutMs  = 20 * 1000;

LoadedScheduledFlight::LoadedScheduledFlight(QObject* parent)
    : QObject(parent)
{
    connect(&_heartbeatTimer, &QTimer::timeout, this, &LoadedScheduledFlight::_sendHeartbeat);
    _heartbeatTimer.start(kHeartbeatMs);
}

/// Random id, unique for each running QGC process
static QString instanceId(void)
{
    static const QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    return id;
}

QStringList LoadedScheduledFlight::parseOtherClaimants(const QByteArray& bytes, const QString& ownInstanceId)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bytes, &parseError);
    if (!jsonDoc.isObject()) {
        qCWarning(LoadedScheduledFlightLog) << "Heartbeat response is not a JSON object:" << parseError.errorString();
        return {};
    }

    QStringList otherClaimants;
    for (const QJsonValue& value : jsonDoc.object()["claims"].toArray()) {
        const QJsonObject claim = value.toObject();
        QString claimInstanceId = claim["instance_id"].toString();
        if (claimInstanceId == ownInstanceId) {
            continue;
        }
        QString agcIdentifier = claim["agc_identifier"].toString();
        otherClaimants.append(agcIdentifier.isEmpty() ? claimInstanceId : agcIdentifier);
    }
    return otherClaimants;
}

void LoadedScheduledFlight::setPlanMasterController(PlanMasterController* planMasterController)
{
    if (_planMasterController) {
        disconnect(_planMasterController, nullptr, this, nullptr);
    }
    _planMasterController = planMasterController;
    if (_planMasterController) {
        connect(_planMasterController, &PlanMasterController::scheduledFlightChanged, this, &LoadedScheduledFlight::_scheduledFlightChanged);
        // `_planMasterController` is already null when this fires, which releases the claim
        connect(_planMasterController, &QObject::destroyed, this, &LoadedScheduledFlight::_scheduledFlightChanged);
    }
    _scheduledFlightChanged();
}

void LoadedScheduledFlight::_scheduledFlightChanged(void)
{
    ScheduledFlight flight = _planMasterController ? _planMasterController->scheduledFlight() : ScheduledFlight();
    if (flight.reference != _flight.reference) {
        _releaseClaim(_flight.reference);
        _setOtherClaimants({});
    }
    _flight = flight;
    _sendHeartbeat();
}

QUrl LoadedScheduledFlight::_flightUrl(const QString& reference, const QString& path)
{
    QString baseUrl = qgcApp()->toolbox()->settingsManager()->aviantSettings()->missionToolsUrl()->rawValue().toString();
    if (baseUrl.isEmpty() || reference.isEmpty()) {
        return QUrl();
    }
    return QUrl(baseUrl + "/api/scheduled-flights/" + QUrl::toPercentEncoding(reference) + "/" + path);
}

void LoadedScheduledFlight::_sendHeartbeat(void)
{
    // A heartbeat still in flight may be for the previous flight
    if (_reply) {
        QNetworkReply* reply = _reply;
        _reply = nullptr;
        reply->abort();
    }

    QUrl url = _flightUrl(_flight.reference, "claim/");
    if (url.isEmpty()) {
        _setOtherClaimants({});
        return;
    }

    QJsonObject body;
    body["instance_id"]     = instanceId();
    body["agc_identifier"]  = qgcApp()->toolbox()->settingsManager()->aviantSettings()->agcIdentifier()->rawValue().toString();

    QNetworkRequest request = AviantMissionTools::createMmsRequest(url);
    request.setTransferTimeout(kRequestTimeoutMs);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = _networkAccessManager.post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { _heartbeatComplete(reply); });
    _reply = reply;
}

void LoadedScheduledFlight::_releaseClaim(const QString& reference)
{
    QUrl url = _flightUrl(reference, "release/");
    if (url.isEmpty()) {
        return;
    }

    QJsonObject body;
    body["instance_id"] = instanceId();

    QNetworkRequest request = AviantMissionTools::createMmsRequest(url);
    request.setTransferTimeout(kRequestTimeoutMs);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = _networkAccessManager.post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        reply->deleteLater();
        // Not fatal, MMS drops the claim once the heartbeats stop
        if (reply->error() != QNetworkReply::NoError) {
            qCWarning(LoadedScheduledFlightLog) << "Releasing scheduled flight claim failed:" << reply->errorString();
        }
    });
}

void LoadedScheduledFlight::_heartbeatComplete(QNetworkReply* reply)
{
    reply->deleteLater();
    if (reply != _reply) {
        // Aborted request
        return;
    }
    _reply = nullptr;

    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(LoadedScheduledFlightLog) << "Scheduled flight heartbeat failed:" << reply->errorString();
        return;
    }

    _setOtherClaimants(parseOtherClaimants(reply->readAll(), instanceId()));
}

void LoadedScheduledFlight::_setOtherClaimants(const QStringList& otherClaimants)
{
    if (otherClaimants == _otherClaimants) {
        return;
    }
    _otherClaimants = otherClaimants;
    emit otherClaimantsChanged();
}
