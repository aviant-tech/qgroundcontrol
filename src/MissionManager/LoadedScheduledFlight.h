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

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QStringList>
#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(LoadedScheduledFlightLog)

/// Watches the scheduled flight the plan was loaded from. Claims it in MMS, and reports other QGC instances that have loaded it
/// too, whether MMS still has the mission plan that was loaded, and whether the flight is ready
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

    QStringList otherClaimants      (void) const { return _otherClaimants; }
    bool        missionPlanOutdated (void) const { return _missionPlanOutdated; }
    QString     notReadyStatus      (void) const { return _notReadyStatus; }
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
    /// `path` under the flight's MMS URL, empty if `reference` or the MMS URL is empty
    static QUrl _flightUrl  (const QString& reference, const QString& path);

    QNetworkAccessManager   _networkAccessManager;
    QTimer                  _heartbeatTimer;
    QNetworkReply*          _reply = nullptr;
    QPointer<PlanMasterController> _planMasterController;
    ScheduledFlight         _flight;
    QStringList             _otherClaimants;
    bool                    _missionPlanOutdated = false;
    QString                 _notReadyStatus;
};
