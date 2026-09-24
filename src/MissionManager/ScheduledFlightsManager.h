/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCLoggingCategory.h"

#include <QObject>
#include <QDateTime>
#include <QGeoCoordinate>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QVariantList>

Q_DECLARE_LOGGING_CATEGORY(ScheduledFlightsManagerLog)

/// Polls MMS for scheduled flights and provides the paths to draw in the fly view
class ScheduledFlightsManager : public QObject
{
    Q_OBJECT

public:
    ScheduledFlightsManager(QObject* parent = nullptr);

    enum FlightState {
        Hidden,
        Soon,   ///< Flight window starts within the lookahead, drawn dashed
        Open    ///< Inside the flight window, drawn solid
    };
    Q_ENUM(FlightState)

    /// Polylines to draw, each a list of `QGeoCoordinate`. Upcoming flights are split into one polyline per dash.
    Q_PROPERTY(QVariantList paths READ paths NOTIFY pathsChanged)

    QVariantList paths(void) const { return _paths; }

    static FlightState           classify        (const QDateTime& start, const QDateTime& end, const QDateTime& now, int lookaheadMinutes);
    /// Cuts `path` at its point farthest from the start, if the rest only retraces the path back
    static QList<QGeoCoordinate> trimReturnTrack (const QList<QGeoCoordinate>& path);
    /// Splits `path` into dashes of `dashMeters`, separated by gaps of `gapMeters`
    static QList<QVariantList>   dashPath        (const QList<QGeoCoordinate>& path, double dashMeters, double gapMeters);

signals:
    void pathsChanged(void);

private slots:
    void _refresh                   (void);
    void _missionToolsUrlChanged    (void);
    void _pollIntervalChanged       (void);
    void _updatePaths               (void);

private:
    struct Flight {
        QDateTime               start;
        QDateTime               end;
        QList<QGeoCoordinate>   path;
    };

    void _requestComplete           (QNetworkReply* reply);
    void _parseResponse             (const QByteArray& bytes);

    QNetworkAccessManager   _networkAccessManager;
    QTimer                  _pollTimer;
    QNetworkReply*          _reply = nullptr;
    QList<Flight>           _flights;
    QList<QPair<FlightState, QList<QGeoCoordinate>>> _visibleFlights;
    QVariantList            _paths;
};
