/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QDateTime>
#include <QGeoCoordinate>
#include <QJsonObject>
#include <QList>
#include <QMetaType>
#include <QString>

/// A scheduled flight from MMS. Default constructed if none, with an empty `reference`
struct ScheduledFlight {
    Q_GADGET

    Q_PROPERTY(QString   reference           MEMBER reference)
    Q_PROPERTY(QString   sourceReference     MEMBER sourceReference)
    Q_PROPERTY(int       missionPlanId       MEMBER missionPlanId)          ///< 0 if none
    Q_PROPERTY(QDateTime start               MEMBER start)
    Q_PROPERTY(QDateTime end                 MEMBER end)
    Q_PROPERTY(QDateTime requestedDeliveryAt MEMBER requestedDeliveryAt)
    Q_PROPERTY(QString   deliveryAddress     MEMBER deliveryAddress)        ///< Street address, empty if none

public:
    QString                 reference;
    QString                 sourceReference;
    int                     missionPlanId = 0;
    QDateTime               start;                  ///< Invalid if missing
    QDateTime               end;                    ///< Invalid if missing
    QDateTime               requestedDeliveryAt;    ///< Invalid if missing
    QString                 deliveryAddress;
    bool                    cancelled = false;
    QList<QGeoCoordinate>   path;

    /// Parses a scheduled flight object as returned by MMS
    static ScheduledFlight fromJson(const QJsonObject& json);
};

Q_DECLARE_METATYPE(ScheduledFlight)
