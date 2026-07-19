/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "SettingsGroup.h"

/// Aviant specific settings.
///
/// Minimal group covering the mission-tools / Kyte / MMS integration (PB7).
/// Later feature groups (PB2/PB3/PB9) extend this group and PB11 reconciles/migrates.
class AviantSettings : public SettingsGroup
{
    Q_OBJECT
public:
    AviantSettings(QObject* parent = nullptr);
    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(missionToolsInsecureHttps)
    DEFINE_SETTINGFACT(missionToolsUrl)
    DEFINE_SETTINGFACT(missionToolsToken)
    DEFINE_SETTINGFACT(kyteBackendUrl)
    DEFINE_SETTINGFACT(kyteBackendToken)
    DEFINE_SETTINGFACT(showTrafficIndicators)
    DEFINE_SETTINGFACT(horizontalConflictDistance)
    DEFINE_SETTINGFACT(verticalConflictDistance)
    DEFINE_SETTINGFACT(multidroneConflictDistance)
    DEFINE_SETTINGFACT(showMultidroneConflictCircle)
    DEFINE_SETTINGFACT(showWinchControlMenu)
    DEFINE_SETTINGFACT(showBatteryWidget)
    DEFINE_SETTINGFACT(preDeliveryBingoLimit)
    DEFINE_SETTINGFACT(postDeliveryBingoLimit)
    DEFINE_SETTINGFACT(persistentConsumeWarnLimit)
    DEFINE_SETTINGFACT(unitUntilThreshold)
};
