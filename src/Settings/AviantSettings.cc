/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AviantSettings.h"

#include <QtQml/QQmlEngine>

DECLARE_SETTINGGROUP(Aviant, "Aviant")
{
    qmlRegisterUncreatableType<AviantSettings>("QGroundControl.SettingsManager", 1, 0, "AviantSettings", "Reference only");
}

DECLARE_SETTINGSFACT(AviantSettings, missionToolsInsecureHttps)
DECLARE_SETTINGSFACT(AviantSettings, missionToolsUrl)
DECLARE_SETTINGSFACT(AviantSettings, missionToolsToken)
DECLARE_SETTINGSFACT(AviantSettings, kyteBackendUrl)
DECLARE_SETTINGSFACT(AviantSettings, kyteBackendToken)
DECLARE_SETTINGSFACT(AviantSettings, showTrafficIndicators)
DECLARE_SETTINGSFACT(AviantSettings, horizontalConflictDistance)
DECLARE_SETTINGSFACT(AviantSettings, verticalConflictDistance)
DECLARE_SETTINGSFACT(AviantSettings, multidroneConflictDistance)
DECLARE_SETTINGSFACT(AviantSettings, showMultidroneConflictCircle)
