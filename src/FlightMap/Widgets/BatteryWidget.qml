import QtQuick          2.12
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4

import QGroundControl                 1.0
import QGroundControl.Controls        1.0
import QGroundControl.ScreenTools     1.0
import QGroundControl.Palette         1.0
import QGroundControl.SettingsManager 1.0
import QGroundControl.FactSystem      1.0
import QGroundControl.FactControls    1.0
import MAVLink                        1.0

Rectangle {
    id:      batteryWidgetContainer
    color:   qgcPal.window
    opacity: 0.9
    radius:  _toolsMargin / 2
    visible: _activeVehicle != undefined

    // Overridden by parent
    property int availableHeight: 600
    
    implicitHeight: Math.min(batteryWidgetColumn.implicitHeight + ScreenTools.defaultFontPixelWidth, availableHeight)

    property var _activeVehicle:  QGroundControl.multiVehicleManager.activeVehicle

    ColumnLayout  {
        id:              batteryWidgetColumn
        anchors.fill:    parent
        anchors.margins: _toolsMargin

        Repeater {
            model: _activeVehicle ? _activeVehicle.batteries : 0

            Loader {
                Layout.fillWidth:       true
                Layout.preferredHeight: item ? item.implicitHeight : 0
                sourceComponent:        batteryWidget

                property var battery:   object
            }
        }
    }

    Component {
        id: batteryWidget

        ColumnLayout {
            spacing:         ScreenTools.defaultFontPixelWidth
            anchors.margins: _toolsMargin
            width:           parent.width

            RowLayout {
                Layout.fillWidth: true

                ColumnLayout {
                    id:               currentContainer
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    height:           parent.height
                    spacing:          ScreenTools.defaultFontPixelWidth / 2
                    visible:          battery.current.valueString != undefined && battery.voltage.valueString != ""

                    QGCLabel {
                        text:  qsTr("Current")
                        color: qgcPal.colorGrey
                    }

                    QGCLabel {
                        font.pointSize: ScreenTools.mediumFontPointSize
                        text:           battery.current.valueString + " " + battery.current.units 
                    }
                }

                Item {
                    Layout.fillWidth: true // Makes the throttle container align to the right
                }

                ColumnLayout {
                    id:               throttleContainer
                    Layout.fillWidth: true
                    height:           parent.height
                    spacing:          ScreenTools.defaultFontPixelWidth / 2
                    visible:          _activeVehicle != undefined && _activeVehicle.throttlePct.valueString != undefined && _activeVehicle.throttlePct.valueString != ""

                    QGCLabel {
                        text:  qsTr("Throttle")
                        color: qgcPal.colorGrey
                    }

                    QGCLabel {
                        font.pointSize: ScreenTools.mediumFontPointSize
                        text:           _activeVehicle.throttlePct.valueString + " " + _activeVehicle.throttlePct.units 
                    }
                }
            }
            RowLayout {
                id:               bottomContainer
                Layout.fillWidth:       true
                Layout.fillHeight:      true

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        id:               voltageContainer
                        Layout.fillWidth: true
                        spacing:          ScreenTools.defaultFontPixelWidth / 2
                        visible:          battery.voltage.valueString != undefined && battery.voltage.valueString != ""
                        QGCLabel {
                            text:  qsTr("Voltage")
                            color: qgcPal.colorGrey
                        }
                        QGCLabel {
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           battery.voltage.valueString + " " + battery.voltage.units 
                        }
                    }

                    ColumnLayout {
                        id:               stateOfChargeContainer
                        Layout.fillWidth: true
                        spacing:          ScreenTools.defaultFontPixelWidth / 2
                        visible:          battery.percentRemaining.valueString != undefined && battery.percentRemaining.valueString != ""
                        QGCLabel {
                            text:  qsTr("SoC")
                            color: qgcPal.colorGrey
                        }
                        QGCLabel {
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           battery.percentRemaining.valueString + " " + battery.percentRemaining.units 
                        }
                    }
                    ColumnLayout {
                        id: nextThresholdInfo
                        spacing: ScreenTools.defaultFontPixelWidth / 2

                        property string nextThreshold: battery ? battery.nextThresholdName.value : ""
                        property real timeUntilNextThreshold: battery ? battery.timeUntilNextThreshold.value : -1

                        visible: nextThreshold !== "" && timeUntilNextThreshold >= 0

                        QGCLabel {
                            Layout.fillWidth: true
                            text:             nextThresholdInfo.nextThreshold
                            color:            qgcPal.colorGrey
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        QGCLabel {
                            Layout.fillWidth: true
                            font.pointSize:   ScreenTools.mediumFontPointSize
                            text: {
                                const minutes = Math.floor(nextThresholdInfo.timeUntilNextThreshold / 60)
                                return qsTr("%1 min").arg(minutes)
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true // Makes the battery visual align to the right
                }

                BatteryVisual {
                    batteryItem:           battery
                    Layout.fillHeight:     true
                    Layout.topMargin:      _toolsMargin
                    Layout.bottomMargin:   _toolsMargin
                }
            }     
        }
    }   

    MouseArea {
        anchors.fill: parent
        onClicked: {
            batteryPopup.open()
        }
    }

    Popup {
        id:             batteryPopup
        padding:        ScreenTools.defaultFontPixelWidth * 0.75
        modal:          true
        focus:          true
        closePolicy:    Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            width:  loader.width
            height: loader.height
            color:  Qt.rgba(0,0,0,0)
        }
        Loader {
            id:              loader
            onLoaded: {
                batteryPopup.x = -(getWidthOfChildren() + _toolsMargin * 2)
            }
            function getWidthOfChildren() {
                // As loader.item is component with width and height 0, we need to access the child item
                if (loader.item.children && loader.item.children.length > 0) {
                    return loader.item.children[0].width
                }
                return 0
            }
        }
        onOpened: {
            loader.sourceComponent = batteryPopupComponent
        }
        onClosed: {
            loader.sourceComponent = null
        }
    }

    Component {
        id: batteryPopupComponent

        BatteryPopup {}
    }
}