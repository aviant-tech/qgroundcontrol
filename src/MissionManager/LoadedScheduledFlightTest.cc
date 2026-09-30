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

void LoadedScheduledFlightTest::_testParseOtherClaimants(void)
{
    QByteArray response = R"({"claims": [
        {"instance_id": "own",   "agc_identifier": "station1", "last_seen": "2026-09-30T10:00:00Z"},
        {"instance_id": "other", "agc_identifier": "station2", "last_seen": "2026-09-30T10:00:00Z"},
        {"instance_id": "unnamed", "agc_identifier": "",       "last_seen": "2026-09-30T10:00:00Z"}
    ]})";
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants(response, "own"), QStringList({ "station2", "unnamed" }));

    // Only our own claim
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants(R"({"claims": [{"instance_id": "own", "agc_identifier": "station1"}]})", "own"), QStringList());

    // Invalid responses
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants("[]", "own"), QStringList());
    QCOMPARE(LoadedScheduledFlight::parseOtherClaimants("not json", "own"), QStringList());
}
