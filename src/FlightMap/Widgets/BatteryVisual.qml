import QtQuick 2.12
import QtQuick.Layouts 1.12
import QGroundControl 1.0
import QGroundControl.Controls 1.0
import QGroundControl.ScreenTools 1.0
import QGroundControl.Palette 1.0
import QGroundControl.SettingsManager       1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.FactControls          1.0

Item {
    id:           root
    width:        50

    property var _aviantSettings: QGroundControl.settingsManager.aviantSettings
    property var batteryItem

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
        property Fact batCapacity:           vehicleReady ? controller.getParameterFact(-1, "BAT1_CAPACITY") : null
        property Fact batCriticalThreshold:  vehicleReady ? controller.getParameterFact(-1, "BAT_CRIT_THR") : null
        property Fact batEmergencyThreshold: vehicleReady ? controller.getParameterFact(-1, "BAT_EMERGEN_THR") : null

        // Use batConsumed / batCapacity instead of SoC because SoC is not very accurate except for the first and last 10% of the battery
        property real percentageRemaining:   batConsumed && batCapacity ? 100 - (batConsumed.rawValue * 100 / batCapacity.rawValue) : 0
        property real margin:                ScreenTools.defaultFontPixelHeight / 4

        border.width: margin

        function calculateBottomMargin(percentage) {
            if (!vehicleReady || percentage === null || isNaN(percentage)) {
                return margin;
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
            border.color:         qgcPal.text
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
            border.color:         qgcPal.text
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