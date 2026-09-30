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

/// Watches the scheduled flight the plan was loaded from. Claims it in MMS, and reports other QGC instances that have loaded it too
class LoadedScheduledFlight : public QObject
{
    Q_OBJECT

public:
    LoadedScheduledFlight(QObject* parent = nullptr);

    /// Controller whose `scheduledFlight` is watched
    Q_PROPERTY(PlanMasterController* planMasterController READ planMasterController WRITE setPlanMasterController)
    /// `agcIdentifier` of each other instance that has loaded the same flight
    Q_PROPERTY(QStringList otherClaimants READ otherClaimants                   NOTIFY otherClaimantsChanged)

    QStringList otherClaimants  (void) const { return _otherClaimants; }
    PlanMasterController* planMasterController      (void) const { return _planMasterController; }
    void                  setPlanMasterController   (PlanMasterController* planMasterController);

    /// `agc_identifier` of each claim in the heartbeat response `bytes` that is not from `ownInstanceId`
    static QStringList parseOtherClaimants (const QByteArray& bytes, const QString& ownInstanceId);

signals:
    void otherClaimantsChanged  (void);

private slots:
    void _scheduledFlightChanged    (void);
    void _sendHeartbeat             (void);

private:
    void _releaseClaim      (const QString& reference);
    void _heartbeatComplete (QNetworkReply* reply);
    void _setOtherClaimants (const QStringList& otherClaimants);
    /// `path` under the flight's MMS URL, empty if `reference` or the MMS URL is empty
    static QUrl _flightUrl  (const QString& reference, const QString& path);

    QNetworkAccessManager   _networkAccessManager;
    QTimer                  _heartbeatTimer;
    QNetworkReply*          _reply = nullptr;
    QPointer<PlanMasterController> _planMasterController;
    ScheduledFlight         _flight;
    QStringList             _otherClaimants;
};
