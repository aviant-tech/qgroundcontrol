/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ScheduledFlight.h"

#include <QJsonArray>

ScheduledFlight ScheduledFlight::fromJson(const QJsonObject& json)
{
    ScheduledFlight flight;
    flight.reference     = json["reference"].toString();
    flight.missionPlanId = json["mission_plan_id"].toInt();
    flight.start         = QDateTime::fromString(json["flight_window_start"].toString(), Qt::ISODate);
    flight.end           = QDateTime::fromString(json["flight_window_end"].toString(), Qt::ISODate);
    flight.cancelled     = json["cancelled"].isString();
    for (const QJsonValue& point : json["flight_path"].toArray()) {
        const QJsonArray lngLat = point.toArray();
        flight.path.append(QGeoCoordinate(lngLat[1].toDouble(), lngLat[0].toDouble()));
    }
    return flight;
}
