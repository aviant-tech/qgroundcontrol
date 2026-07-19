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
import MAVLink

// Aviant (A53/A63): Detailed battery popup showing per-battery remaining/consumed/temperature plus
// the configured bingo and PX4 failsafe limits, with a reset for the persistent consumed offset.
Item {
    id: batteryPopupContainer

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    QGCPalette { id: qgcPal }

    Component {
        id: batteryValuesAvailableComponent

        QtObject {
            property bool temperatureAvailable:   !isNaN(battery.temperature.rawValue)
            property bool currentAvailable:       !isNaN(battery.current.rawValue)
            property bool mahConsumedAvailable:   !isNaN(battery.mahConsumed.rawValue)
            property bool timeRemainingAvailable: !isNaN(battery.timeRemaining.rawValue)
            property bool chargeStateAvailable:   battery.chargeState.rawValue !== MAVLink.MAV_BATTERY_CHARGE_STATE_UNDEFINED
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

                            QGCLabel {
                                text:    qsTr("Battery %1").arg(object.id.rawValue)
                                visible: _activeVehicle.batteries.count > 1
                            }
                            QGCLabel { text: qsTr("Remaining") }
                            QGCLabel {
                                text:    object.hasPersistedConsumed ? qsTr("Persistent consumed") : qsTr("Consumed")
                                visible: batteryValuesAvailable.mahConsumedAvailable
                            }
                            QGCLabel {
                                text:    qsTr("Temperature")
                                visible: batteryValuesAvailable.temperatureAvailable
                            }
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

                            QGCLabel { text: ""; visible: _activeVehicle.batteries.count > 1 }
                            QGCLabel { text: object.consumedBasedRemaining.valueString + " " + object.consumedBasedRemaining.units }
                            QGCLabel { text: object.mahConsumed.valueString + " " + object.mahConsumed.units; visible: batteryValuesAvailable.mahConsumedAvailable }
                            QGCLabel { text: object.temperature.valueString + " " + object.temperature.units;  visible: batteryValuesAvailable.temperatureAvailable }
                        }
                    }
                }
            }

            QGCButton {
                text:             qsTr("Reset persisted consumed")
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    if (_activeVehicle) {
                        _activeVehicle.resetPersistedConsumedData()
                    }
                }
                visible: _activeVehicle && _activeVehicle.hasPersistedConsumedData()
            }

            QGCLabel {
                Layout.alignment: Qt.AlignCenter
                text:             qsTr("Limits")
                font.family:      ScreenTools.demiboldFontFamily
            }

            QGCLabel {
                visible: _activeVehicle && _activeVehicle.hasPersistedConsumedData()
                text:    "NB! When flying multiple\n"
                         + "flights on the same battery,\n"
                         + "failsafe limits will not\n"
                         + "work as intended. Manual\n"
                         + "intervention will be needed."
            }

            RowLayout {
                id:      limitsRow
                spacing: ScreenTools.defaultFontPixelWidth

                FactPanelController { id: controller }

                property var _aviantSettings: QGroundControl.settingsManager.aviantSettings

                property Fact preDeliveryBingoLimit:  _aviantSettings.preDeliveryBingoLimit
                property Fact postDeliveryBingoLimit: _aviantSettings.postDeliveryBingoLimit
                property Fact batCapacity:            controller.parameterExists(-1, "BAT1_CAPACITY")   ? controller.getParameterFact(-1, "BAT1_CAPACITY", false)   : null
                property Fact batCriticalThreshold:   controller.parameterExists(-1, "BAT_CRIT_THR")    ? controller.getParameterFact(-1, "BAT_CRIT_THR", false)    : null
                property Fact batEmergencyThreshold:  controller.parameterExists(-1, "BAT_EMERGEN_THR") ? controller.getParameterFact(-1, "BAT_EMERGEN_THR", false) : null

                ColumnLayout {
                    QGCLabel { text: limitsRow.preDeliveryBingoLimit.name }
                    QGCLabel { text: limitsRow.postDeliveryBingoLimit.name }
                    QGCLabel {
                        text:  limitsRow.batCriticalThreshold ? limitsRow.batCriticalThreshold.name : "N/A"
                        color: qgcPal.colorOrange
                    }
                    QGCLabel {
                        text:  limitsRow.batEmergencyThreshold ? limitsRow.batEmergencyThreshold.name : "N/A"
                        color: qgcPal.colorRed
                    }
                    QGCLabel {
                        text: limitsRow.batCapacity ? limitsRow.batCapacity.name : "N/A"
                    }
                }
                ColumnLayout {
                    QGCLabel {
                        text: limitsRow.preDeliveryBingoLimit ? limitsRow.preDeliveryBingoLimit.rawValue + " " + limitsRow.preDeliveryBingoLimit.units : "N/A"
                    }
                    QGCLabel {
                        text: limitsRow.postDeliveryBingoLimit ? limitsRow.postDeliveryBingoLimit.rawValue + " " + limitsRow.postDeliveryBingoLimit.units : "N/A"
                    }
                    QGCLabel {
                        text: limitsRow.batCriticalThreshold && limitsRow.batCapacity ?
                            parent.thresholdPercentageToMah(limitsRow.batCriticalThreshold.rawValue, limitsRow.batCapacity.rawValue) + " " + limitsRow.batCapacity.units :
                            "N/A"
                        color: qgcPal.colorOrange
                    }
                    QGCLabel {
                        text: limitsRow.batEmergencyThreshold && limitsRow.batCapacity ?
                            parent.thresholdPercentageToMah(limitsRow.batEmergencyThreshold.rawValue, limitsRow.batCapacity.rawValue) + " " + limitsRow.batCapacity.units :
                            "N/A"
                        color: qgcPal.colorRed
                    }
                    QGCLabel {
                        text: limitsRow.batCapacity ? limitsRow.batCapacity.rawValue + " " + limitsRow.batCapacity.units : "N/A"
                    }

                    function thresholdPercentageToMah(remainingThresholdPercentage, capacityMah) {
                        const consumedThreshold = 1 - remainingThresholdPercentage
                        return (capacityMah * consumedThreshold).toFixed(0)
                    }
                }
            }
        }
    }
}
