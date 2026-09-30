/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "LoadedScheduledFlightTest.h"
#include "LoadedScheduledFlight.h"

#include <QJsonDocument>

static QJsonObject toJson(const char* json)
{
    return QJsonDocument::fromJson(json).object();
}

void LoadedScheduledFlightTest::_testParseOtherClaimants(void)
{
    QJsonObject response = toJson(R"({"claims": [
        {"instance_id": "own",   "agc_identifier": "station1", "last_seen": "2026-09-30T10:00:00Z"},
        {"instance_id": "other", "agc_identifier": "station2", "last_seen": "2026-09-30T10:00:00Z"},
        {"instance_id": "unnamed", "agc_identifier": "",       "last_seen": "2026-09-30T10:00:00Z"}
    ]})");
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants(response, "own"), QStringList({ "station2", "unnamed" }));

    // Only our own claim
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants(toJson(R"({"claims": [{"instance_id": "own", "agc_identifier": "station1"}]})"), "own"), QStringList());

    // No claims
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants(QJsonObject(), "own"), QStringList());
}

void LoadedScheduledFlightTest::_testStatusIfNotReady(void)
{
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady("READY"), QString());
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady("ready"), QString());
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady("CANCELLED"), QString("CANCELLED"));
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady("pending"), QString("pending"));
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady(" READY\n"), QString());
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady(" FAILED "), QString("FAILED"));
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady("  "), QString());
    // MMS without `status`
    QCOMPARE(LoadedScheduledFlight::statusIfNotReady(""), QString());
}
