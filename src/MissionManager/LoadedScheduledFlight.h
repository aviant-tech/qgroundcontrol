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
#include "PlanMasterController.h"
#include "ScheduledFlight.h"

#include <QDateTime>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QStringList>
#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(LoadedScheduledFlightLog)

/// Watches the scheduled flight the plan was loaded from. Claims it in MMS, and reports other QGC instances that have loaded it
/// too, whether MMS still has the mission plan that was loaded, whether the flight is ready, and whether MMS can be reached
class LoadedScheduledFlight : public QObject
{
    Q_OBJECT

public:
    LoadedScheduledFlight(QObject* parent = nullptr);

    /// Controller whose `scheduledFlight` is watched
    Q_PROPERTY(PlanMasterController* planMasterController READ planMasterController WRITE setPlanMasterController)
    /// `agcIdentifier` of each other instance that has loaded the same flight
    Q_PROPERTY(QStringList otherClaimants READ otherClaimants                   NOTIFY otherClaimantsChanged)
    /// MMS has another mission plan, or none, for the flight than the one loaded
    Q_PROPERTY(bool        missionPlanOutdated READ missionPlanOutdated         NOTIFY missionPlanOutdatedChanged)
    /// The flight's MMS status if it is not READY, e.g. CANCELLED, or DELETED if MMS no longer has the flight. Empty if ready or unknown
    Q_PROPERTY(QString     notReadyStatus READ notReadyStatus                   NOTIFY notReadyStatusChanged)
    /// No valid heartbeat response from MMS for a while, whatever the reason, so changes to the flight in MMS, e.g. a cancellation or new mission plan, are not detected
    Q_PROPERTY(bool        mmsUnreachable READ mmsUnreachable                   NOTIFY mmsUnreachableChanged)
    /// Time of the last valid heartbeat response, or of loading the flight if none since. Invalid before a flight is loaded
    Q_PROPERTY(QDateTime   lastMmsResponse READ lastMmsResponse                 NOTIFY lastMmsResponseChanged)

    QStringList otherClaimants      (void) const { return _otherClaimants; }
    bool        missionPlanOutdated (void) const { return _missionPlanOutdated; }
    QString     notReadyStatus      (void) const { return _notReadyStatus; }
    bool        mmsUnreachable      (void) const { return _mmsUnreachable; }
    QDateTime   lastMmsResponse     (void) const { return _lastMmsResponse; }
    PlanMasterController* planMasterController      (void) const { return _planMasterController; }
    void                  setPlanMasterController   (PlanMasterController* planMasterController);

    /// `agc_identifier` of each claim in the heartbeat response `json` that is not from `ownInstanceId`
    static QStringList parseOtherClaimants  (const QJsonObject& json, const QString& ownInstanceId);
    /// Trimmed `status` unless it is READY, ignoring case. Empty `status` gives empty, so an MMS without it never warns
    static QString     statusIfNotReady     (const QString& status);

signals:
    void otherClaimantsChanged      (void);
    void missionPlanOutdatedChanged (void);
    void notReadyStatusChanged      (void);
    void mmsUnreachableChanged      (void);
    void lastMmsResponseChanged     (void);

private slots:
    void _scheduledFlightChanged    (void);
    void _sendHeartbeat             (void);

private:
    void _releaseClaim      (const QString& reference);
    void _heartbeatComplete (QNetworkReply* reply);
    /// Clears everything learned from heartbeat responses
    void _clearWarnings         (void);
    void _setOtherClaimants     (const QStringList& otherClaimants);
    void _setMissionPlanOutdated(bool missionPlanOutdated);
    void _setNotReadyStatus     (const QString& notReadyStatus);
    /// Restarts the watchdog, updates `lastMmsResponse` and clears `mmsUnreachable`
    void _mmsReached            (void);
    void _setMmsUnreachable     (bool mmsUnreachable);
    /// `path` under the flight's MMS URL, empty if `reference` or the MMS URL is empty
    static QUrl _flightUrl  (const QString& reference, const QString& path);

    QNetworkAccessManager   _networkAccessManager;
    QTimer                  _heartbeatTimer;
    QTimer                  _watchdogTimer;         ///< Sets `mmsUnreachable` unless restarted by a heartbeat response
    QNetworkReply*          _reply = nullptr;
    QPointer<PlanMasterController> _planMasterController;
    ScheduledFlight         _flight;
    QStringList             _otherClaimants;
    bool                    _missionPlanOutdated = false;
    QString                 _notReadyStatus;
    bool                    _mmsUnreachable = false;
    QDateTime               _lastMmsResponse;
};
