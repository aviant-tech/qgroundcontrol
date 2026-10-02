/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ScheduledFlightsManagerTest.h"
#include "ScheduledFlightsManager.h"

#include <QJsonDocument>

static const QGeoCoordinate kStart(59.708188, 10.884887);

static double dashLength(const QVariantList& dash)
{
    double length = 0;
    for (int i = 1; i < dash.count(); i++) {
        length += dash[i - 1].value<QGeoCoordinate>().distanceTo(dash[i].value<QGeoCoordinate>());
    }
    return length;
}

void ScheduledFlightsManagerTest::_testParseFlight(void)
{
    QJsonObject json = QJsonDocument::fromJson(R"({
        "reference": "8fbe201b", "source_reference": "source-reference-1", "status": "READY", "mission_plan_id": 695, "cancelled": null,
        "flight_window_start": "2026-09-22T09:30:00Z", "flight_window_end": "2026-09-22T10:30:00Z",
        "requested_delivery_at": "2026-09-22T10:00:00.074034Z",
        "delivery_address": {"street_address": "Solliveien 2", "postal_code": "1825"},
        "flight_path": [[10.88, 59.70], [11.01, 59.66]]
    })").object();

    ScheduledFlight flight = ScheduledFlight::fromJson(json);
    QCOMPARE(flight.reference, QString("8fbe201b"));
    QCOMPARE(flight.sourceReference, QString("source-reference-1"));
    QCOMPARE(flight.status, QString("READY"));
    QCOMPARE(flight.missionPlanId, 695);
    QCOMPARE(flight.start, QDateTime(QDate(2026, 9, 22), QTime(9, 30), Qt::UTC));
    QCOMPARE(flight.end, QDateTime(QDate(2026, 9, 22), QTime(10, 30), Qt::UTC));
    QCOMPARE(flight.requestedDeliveryAt, QDateTime(QDate(2026, 9, 22), QTime(10, 0, 0, 74), Qt::UTC));
    QCOMPARE(flight.deliveryAddress, QString("Solliveien 2"));
    QCOMPARE(flight.cancelled, false);
    QCOMPARE(flight.path, QList<QGeoCoordinate>({ QGeoCoordinate(59.70, 10.88), QGeoCoordinate(59.66, 11.01) }));

    json["cancelled"] = "2026-09-22T09:00:00Z";
    QCOMPARE(ScheduledFlight::fromJson(json).cancelled, true);

    // Missing fields
    ScheduledFlight empty = ScheduledFlight::fromJson(QJsonObject());
    QVERIFY(empty.reference.isEmpty());
    QCOMPARE(empty.missionPlanId, 0);
    QVERIFY(!empty.start.isValid());
    QVERIFY(!empty.end.isValid());
    QVERIFY(empty.path.isEmpty());
}

void ScheduledFlightsManagerTest::_testClassify(void)
{
    QDateTime start = QDateTime::fromString("2026-09-22T09:30:00Z", Qt::ISODate);
    QDateTime end   = QDateTime::fromString("2026-09-22T10:30:00Z", Qt::ISODate);

    QCOMPARE(ScheduledFlightsManager::classify(start, end, start.addSecs(-61 * 60), 60), ScheduledFlightsManager::Hidden);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, start.addSecs(-60 * 60), 60), ScheduledFlightsManager::Soon);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, start.addSecs(-1),       60), ScheduledFlightsManager::Soon);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, start,                   60), ScheduledFlightsManager::Open);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, end.addSecs(-1),         60), ScheduledFlightsManager::Open);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, end,                     60), ScheduledFlightsManager::Hidden);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, end.addSecs(60),         60), ScheduledFlightsManager::Hidden);
    QCOMPARE(ScheduledFlightsManager::classify(start, end, start.addSecs(-1),       0),  ScheduledFlightsManager::Hidden);
}

void ScheduledFlightsManagerTest::_testTrimReturnTrack(void)
{
    QGeoCoordinate p1       = kStart.atDistanceAndAzimuth(500, 90);
    QGeoCoordinate p2       = p1.atDistanceAndAzimuth(500, 45);
    QGeoCoordinate delivery = p2.atDistanceAndAzimuth(500, 90);

    // Out and back with duplicates at the turnaround, return leg stopping short of the start
    QList<QGeoCoordinate> outAndBack = { kStart, p1, p2, delivery, delivery, delivery, p2, p1 };
    QList<QGeoCoordinate> outbound = { kStart, p1, p2, delivery };
    QCOMPARE(ScheduledFlightsManager::trimReturnTrack(outAndBack), outbound);

    // Return leg along a different route is kept
    QList<QGeoCoordinate> loop = { kStart, p1, p2, delivery, delivery.atDistanceAndAzimuth(500, 180), kStart };
    QCOMPARE(ScheduledFlightsManager::trimReturnTrack(loop), loop);

    // One way
    QCOMPARE(ScheduledFlightsManager::trimReturnTrack(outbound), outbound);
}

void ScheduledFlightsManagerTest::_testDashPath(void)
{
    // Straight line: dashes at 0-100, 200-300 and 400-450
    QGeoCoordinate end = kStart.atDistanceAndAzimuth(450, 0);
    QList<QVariantList> dashes = ScheduledFlightsManager::dashPath({ kStart, end }, 100, 100);
    QCOMPARE(dashes.count(), 3);
    QCOMPARE(dashes[0].first().value<QGeoCoordinate>(), kStart);
    QVERIFY(qAbs(dashLength(dashes[0]) - 100) < 0.01);
    QVERIFY(qAbs(kStart.distanceTo(dashes[1].first().value<QGeoCoordinate>()) - 200) < 0.01);
    QVERIFY(qAbs(dashLength(dashes[2]) - 50) < 0.01);
    QCOMPARE(dashes[2].last().value<QGeoCoordinate>(), end);

    // Dash across a vertex: second dash is 120-220, with the vertex at 150
    QGeoCoordinate vertex = kStart.atDistanceAndAzimuth(150, 0);
    dashes = ScheduledFlightsManager::dashPath({ kStart, vertex, vertex.atDistanceAndAzimuth(150, 90) }, 100, 20);
    QCOMPARE(dashes[1].count(), 3);
    QCOMPARE(dashes[1][1].value<QGeoCoordinate>(), vertex);
    QVERIFY(qAbs(dashLength(dashes[1]) - 100) < 0.01);

    // Duplicate points do not end up in the dashes
    QGeoCoordinate mid = kStart.atDistanceAndAzimuth(150, 0);
    dashes = ScheduledFlightsManager::dashPath({ kStart, kStart, mid, mid, end }, 100, 100);
    QCOMPARE(dashes.count(), 3);
    QCOMPARE(dashes[0].count(), 2);
    QCOMPARE(dashes[1].count(), 2);
    QCOMPARE(dashes[2].count(), 2);

    // Path shorter than a dash is one dash
    QGeoCoordinate near = kStart.atDistanceAndAzimuth(50, 0);
    dashes = ScheduledFlightsManager::dashPath({ kStart, near }, 100, 100);
    QCOMPARE(dashes.count(), 1);
    QCOMPARE(dashes[0].count(), 2);
    QCOMPARE(dashes[0][0].value<QGeoCoordinate>(), kStart);
    QCOMPARE(dashes[0][1].value<QGeoCoordinate>(), near);
}
