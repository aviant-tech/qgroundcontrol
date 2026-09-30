/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "UnitTest.h"

class LoadedScheduledFlightTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _testParseOtherClaimants   (void);
};
