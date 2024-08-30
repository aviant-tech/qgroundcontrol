import QtQuick 2.11
import QtQuick.Layouts 1.11
import QGroundControl.Controls 1.0
import QGroundControl.ScreenTools 1.0
import QGroundControl.Palette 1.0
import MAVLink 1.0

Item {
    id: batteryPopupContainer

    Component {
        id: batteryValuesAvailableComponent

        QtObject {
            property bool functionAvailable:        battery.function.rawValue !== MAVLink.MAV_BATTERY_FUNCTION_UNKNOWN
            property bool temperatureAvailable:     !isNaN(battery.temperature.rawValue)
            property bool currentAvailable:         !isNaN(battery.current.rawValue)
            property bool mahConsumedAvailable:     !isNaN(battery.mahConsumed.rawValue)
            property bool timeRemainingAvailable:   !isNaN(battery.timeRemaining.rawValue)
            property bool chargeStateAvailable:     battery.chargeState.rawValue !== MAVLink.MAV_BATTERY_CHARGE_STATE_UNDEFINED
        }
    }

    Rectangle {
        width:        mainLayout.width + mainLayout.anchors.margins * 2
        height:       mainLayout.height + mainLayout.anchors.margins * 2
        radius:       ScreenTools.defaultFontPixelHeight / 2
        color:        qgcPal.window
        border.color: qgcPal.text

        ColumnLayout {
            id:              mainLayout
            anchors.margins: ScreenTools.defaultFontPixelWidth
            anchors.top:     parent.top
            anchors.right:   parent.right
            spacing:         ScreenTools.defaultFontPixelHeight

            QGCLabel {
                Layout.alignment: Qt.AlignCenter
                text:             qsTr("Battery Status")
                font.family:      ScreenTools.demiboldFontFamily
            }

            RowLayout {
                spacing: ScreenTools.defaultFontPixelWidth

                ColumnLayout {
                    Repeater {
                        model: _activeVehicle ? _activeVehicle.batteries : 0

                        ColumnLayout {
                            spacing: 0

                            property var batteryValuesAvailable: nameAvailableLoader.item

                            Loader {
                                id:              nameAvailableLoader
                                sourceComponent: batteryValuesAvailableComponent

                                property var battery: object
                            }

                            QGCLabel { text: qsTr("Battery %1").arg(object.id.rawValue) }
                            QGCLabel { text: qsTr("Charge State");   visible: batteryValuesAvailable.chargeStateAvailable }
                            QGCLabel { text: qsTr("Remaining");      visible: batteryValuesAvailable.timeRemainingAvailable }
                            QGCLabel { text: qsTr("Remaining") }
                            QGCLabel { text: qsTr("Voltage") }
                            QGCLabel { text: qsTr("Consumed");       visible: batteryValuesAvailable.mahConsumedAvailable }
                            QGCLabel { text: qsTr("Temperature");    visible: batteryValuesAvailable.temperatureAvailable }
                            QGCLabel { text: qsTr("Function");       visible: batteryValuesAvailable.functionAvailable }
                        }
                    }
                }

                ColumnLayout {
                    Repeater {
                        model: _activeVehicle ? _activeVehicle.batteries : 0

                        ColumnLayout {
                            spacing: 0

                            property var batteryValuesAvailable: valueAvailableLoader.item

                            Loader {
                                id:              valueAvailableLoader
                                sourceComponent: batteryValuesAvailableComponent

                                property var battery: object
                            }

                            QGCLabel { text: "" }
                            QGCLabel { text: object.chargeState.enumStringValue;                              visible: batteryValuesAvailable.chargeStateAvailable }
                            QGCLabel { text: object.timeRemainingStr.value;                                   visible: batteryValuesAvailable.timeRemainingAvailable }
                            QGCLabel { text: object.percentRemaining.valueString + " " + object.percentRemaining.units }
                            QGCLabel { text: object.voltage.valueString + " " + object.voltage.units }
                            QGCLabel { text: object.mahConsumed.valueString + " " + object.mahConsumed.units; visible: batteryValuesAvailable.mahConsumedAvailable }
                            QGCLabel { text: object.temperature.valueString + " " + object.temperature.units; visible: batteryValuesAvailable.temperatureAvailable }
                            QGCLabel { text: object.function.enumStringValue;                                 visible: batteryValuesAvailable.functionAvailable }
                        }
                    }
                }
            }
        }
    }

}
