import QtQuick 2.11
import QtQuick.Layouts 1.11
import QGroundControl 1.0
import QGroundControl.Controls 1.0
import QGroundControl.ScreenTools 1.0
import QGroundControl.Palette 1.0
import QGroundControl.SettingsManager 1.0
import QGroundControl.FactSystem      1.0
import QGroundControl.FactControls    1.0
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

                            QGCLabel {
                                text:     qsTr("Battery %1").arg(object.id.rawValue)
                                visible:  _activeVehicle.batteries > 1
                            }
                            QGCLabel { text: qsTr("Remaining") }
                            QGCLabel {
                                text:     qsTr("Consumed")
                                visible:  batteryValuesAvailable.mahConsumedAvailable && !object.hasPersistedConsumed
                            }
                            QGCLabel {
                                text:     qsTr("Persistent consumed")
                                visible:  batteryValuesAvailable.mahConsumedAvailable && object.hasPersistedConsumed
                            }
                            QGCLabel {
                                text:     qsTr("Temperature")
                                visible:  batteryValuesAvailable.temperatureAvailable
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

                            QGCLabel { text: ""; visible: _activeVehicle.batteries > 1}
                            QGCLabel { text: object.consumedBasedRemaining.valueString + " " + object.consumedBasedRemaining.units }
                            QGCLabel { text: object.mahConsumed.valueString + " " + object.mahConsumed.units; visible: batteryValuesAvailable.mahConsumedAvailable }
                            QGCLabel { text: object.temperature.valueString + " " + object.temperature.units; visible: batteryValuesAvailable.temperatureAvailable }
                        }
                    }
                }
            }
            QGCButton {
                text: qsTr("Reset persisted consumed")
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    if (_activeVehicle) {
                        _activeVehicle.resetPersistedConsumedData(_activeVehicle.id)
                    }
                }
                visible: _activeVehicle && _activeVehicle.hasPersistedConsumedData(_activeVehicle.id)
            }
            QGCLabel {
                Layout.alignment: Qt.AlignCenter
                text:             qsTr("Limits")
                font.family:      ScreenTools.demiboldFontFamily
            }
            
            QGCLabel {
                visible:  _activeVehicle && _activeVehicle.hasPersistedConsumedData(_activeVehicle.id)
                text:     "NB! When flying multiple\n"
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
                property Fact batCapacity:            controller.getParameterFact(-1, "BAT1_CAPACITY")
                property Fact batCriticalThreshold:   controller.getParameterFact(-1, "BAT_CRIT_THR")
                property Fact batEmergencyThreshold:  controller.getParameterFact(-1, "BAT_EMERGEN_THR")

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
