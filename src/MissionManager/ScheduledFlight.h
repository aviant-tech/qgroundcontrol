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
#include <QString>

/// A scheduled flight from MMS
struct ScheduledFlight {
    QString                 reference;
    int                     missionPlanId = 0;
    QDateTime               start;      ///< Invalid if missing
    QDateTime               end;        ///< Invalid if missing
    bool                    cancelled = false;
    QList<QGeoCoordinate>   path;

    /// Parses a scheduled flight object as returned by MMS
    static ScheduledFlight fromJson(const QJsonObject& json);
};
