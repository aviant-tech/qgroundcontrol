/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls

// Aviant (A53/A63): Vertical battery bar showing consumed-based remaining charge with bingo and
// PX4 critical/emergency threshold markers.
Item {
    id:    root
    width: 50

    property var _aviantSettings: QGroundControl.settingsManager.aviantSettings
    property var batteryItem

    QGCPalette { id: qgcPal }

    Rectangle {
        id:           batteryContainer
        anchors.fill: parent
        color:        "transparent"
        border.color: qgcPal.text
        radius:       width * 0.15

        FactPanelController { id: controller }

        property bool vehicleReady: QGroundControl.multiVehicleManager.activeVehicle
                                    && QGroundControl.multiVehicleManager.activeVehicle.initialConnectComplete
        property Fact batConsumed:           batteryItem ? batteryItem.mahConsumed : null
        property Fact batCapacity:           vehicleReady && controller.parameterExists(-1, "BAT1_CAPACITY")   ? controller.getParameterFact(-1, "BAT1_CAPACITY", false)   : null
        property Fact batCriticalThreshold:  vehicleReady && controller.parameterExists(-1, "BAT_CRIT_THR")    ? controller.getParameterFact(-1, "BAT_CRIT_THR", false)    : null
        property Fact batEmergencyThreshold: vehicleReady && controller.parameterExists(-1, "BAT_EMERGEN_THR") ? controller.getParameterFact(-1, "BAT_EMERGEN_THR", false) : null

        // Use batConsumed / batCapacity instead of SoC because SoC is not very accurate except for the first and last 10% of the battery
        property real percentageRemaining:   batConsumed && batCapacity ? 100 - (batConsumed.rawValue * 100 / batCapacity.rawValue) : 0
        property real margin:                ScreenTools.defaultFontPixelHeight / 4

        border.width: margin

        function calculateBottomMargin(percentage) {
            if (!vehicleReady || percentage === null || isNaN(percentage)) {
                return margin
            }
            return margin + (height - margin * 2) * (percentage)
        }

        Rectangle {
            id:                       batteryTip
            width:                    parent.width * 0.4
            height:                   parent.height * 0.05
            anchors.bottom:           parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            color:                    qgcPal.text
        }

        Rectangle {
            id:              batteryFill
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.bottom:  parent.bottom
            anchors.margins: parent.margin
            height:          (parent.height - (parent.margin * 2)) * (batteryContainer.percentageRemaining / 100)
            color:           "#00E04b" // This color of green gives better contrast with the orange line than qgcPal.colorGreen
            opacity:         0.8
        }

        Rectangle {
            id:                   bingoLimit1Bar
            color:                qgcPal.text
            anchors.left:         parent.left
            anchors.right:        parent.right
            anchors.bottom:       parent.bottom
            anchors.bottomMargin: parent.batCapacity ? parent.calculateBottomMargin(1 - (_aviantSettings.preDeliveryBingoLimit.rawValue / parent.batCapacity.rawValue)) : 0
            anchors.leftMargin:   parent.margin
            anchors.rightMargin:  parent.margin
            height:               ScreenTools.defaultFontPixelHeight / 6
            visible:              parent.batCapacity !== null
        }

        Rectangle {
            id:                   bingoLimit2Bar
            color:                qgcPal.text
            anchors.left:         parent.left
            anchors.right:        parent.right
            anchors.bottom:       parent.bottom
            anchors.bottomMargin: parent.batCapacity ? parent.calculateBottomMargin(1 - (_aviantSettings.postDeliveryBingoLimit.rawValue / parent.batCapacity.rawValue)) : 0
            anchors.leftMargin:   parent.margin
            anchors.rightMargin:  parent.margin
            height:               ScreenTools.defaultFontPixelHeight / 6
            visible:              batteryContainer.batCapacity !== null
        }

        Rectangle {
            id:                   criticalThresholdBar
            anchors.left:         parent.left
            anchors.right:        parent.right
            anchors.bottom:       parent.bottom
            anchors.bottomMargin: batteryContainer.batCriticalThreshold ? parent.calculateBottomMargin(batteryContainer.batCriticalThreshold.rawValue) : 0
            anchors.leftMargin:   parent.margin
            anchors.rightMargin:  parent.margin
            height:               ScreenTools.defaultFontPixelHeight / 6
            color:                "#DD6504"
            visible:              batteryContainer.batCriticalThreshold !== null
        }

        Rectangle {
            id:                   emergencyThresholdBar
            anchors.left:         parent.left
            anchors.right:        parent.right
            anchors.bottom:       parent.bottom
            anchors.bottomMargin: batteryContainer.batEmergencyThreshold ? parent.calculateBottomMargin(batteryContainer.batEmergencyThreshold.rawValue) : 0
            anchors.leftMargin:   parent.margin
            anchors.rightMargin:  parent.margin
            height:               ScreenTools.defaultFontPixelHeight / 6
            color:                qgcPal.colorRed
            visible:              batteryContainer.batEmergencyThreshold !== null
        }
    }
}
