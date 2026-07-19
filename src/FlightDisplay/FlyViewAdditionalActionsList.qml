/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQml

import QGroundControl

QtObject {
    property var guidedController

    property var  _flyViewSettings:     QGroundControl.settingsManager.flyViewSettings
    property real _stepUp:              _flyViewSettings.guidedStepUpAltitude.rawValue
    property real _stepDown:            _flyViewSettings.guidedStepDownAltitude.rawValue

    function _stepAltTitle(deltaMeters, dir) {
        return qsTr("Adjust ") + QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(deltaMeters).toFixed(1)
                + " " + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString + " " + dir
    }

    property bool anyActionAvailable: guidedController.showStartMission || guidedController.showContinueMission || guidedController.showChangeAlt ||
                                      guidedController.showChangeLoiterRadius ||  guidedController.showLandAbort || guidedController.showChangeSpeed ||
                                      guidedController.showGripper
    property var model: [
        {
            // A29: larger (2x) up step
            title:      _stepAltTitle(_stepUp * 2, qsTr("up")),
            text:       guidedController.stepUpAltMessage,
            action:     guidedController.actionStepAlt,
            actionData: _stepUp * 2,
            visible:    guidedController.showChangeAlt
        },
        {
            // A25: fixed step up
            title:      _stepAltTitle(_stepUp, qsTr("up")),
            text:       guidedController.stepUpAltMessage,
            action:     guidedController.actionStepAlt,
            actionData: _stepUp,
            visible:    guidedController.showChangeAlt
        },
        {
            // A25: fixed step down
            title:      _stepAltTitle(_stepDown, qsTr("down")),
            text:       guidedController.stepDownAltMessage,
            action:     guidedController.actionStepAlt,
            actionData: -_stepDown,
            visible:    guidedController.showChangeAlt
        },
        {
            title:      guidedController.startMissionTitle,
            text:       guidedController.startMissionMessage,
            action:     guidedController.actionStartMission,
            visible:    guidedController.showStartMission
        },
        {
            title:      guidedController.continueMissionTitle,
            text:       guidedController.continueMissionMessage,
            action:     guidedController.actionContinueMission,
            visible:    guidedController.showContinueMission
        },
        {
            title:      guidedController.changeAltTitle,
            text:       guidedController.changeAltMessage,
            action:     guidedController.actionChangeAlt,
            visible:    guidedController.showChangeAlt
        },
        {
            title:      guidedController.changeLoiterRadiusTitle,
            text:       guidedController.changeLoiterRadiusMessage,
            action:     guidedController.actionChangeLoiterRadius,
            visible:    guidedController.showChangeLoiterRadius
        },
        {
            title:      guidedController.landAbortTitle,
            text:       guidedController.landAbortMessage,
            action:     guidedController.actionLandAbort,
            visible:    guidedController.showLandAbort
        },
        {
            title:      guidedController.changeSpeedTitle,
            text:       guidedController.changeSpeedMessage,
            action:     guidedController.actionChangeSpeed,
            visible:    guidedController.showChangeSpeed
        },
        {
            title:      guidedController.gripperTitle,
            text:       guidedController.gripperMessage,
            action:     guidedController.actionGripper,
            visible:    guidedController.showGripper
        }
    ]
}
