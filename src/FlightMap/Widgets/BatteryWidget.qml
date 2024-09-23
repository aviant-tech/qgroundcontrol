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

    // Overridden by parent
    property int availableHeight:      600
    property bool winchControlVisible: false
    
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
                sourceComponent:        battery ? batteryWidget : null

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
                    visible:          battery && battery.current.valueString != undefined && battery.voltage.valueString != ""

                    QGCLabel {
                        text:  qsTr("Current")
                        color: qgcPal.colorGrey
                    }

                    QGCLabel {
                        font.pointSize: ScreenTools.mediumFontPointSize
                        text:           battery ? battery.current.valueString + " " + battery.current.units : "N/A"
                    }
                }

                Item {
                    Layout.fillWidth: true // Makes the throttle container align to the right
                }

                ColumnLayout {
                    id:               throttleContainer
                    visible:          _activeVehicle != undefined && _activeVehicle.throttlePct.valueString != undefined && _activeVehicle.throttlePct.valueString != ""

                    QGCLabel {
                        text:  qsTr("Throttle")
                        color: qgcPal.colorGrey
                    }

                    QGCLabel {
                        font.pointSize: ScreenTools.mediumFontPointSize
                        text:           _activeVehicle ? _activeVehicle.throttlePct.valueString + " " + _activeVehicle.throttlePct.units : "N/A" 
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
                        spacing:          ScreenTools.defaultFontPixelWidth / 6
                        visible:          battery && battery.voltage.valueString != undefined && battery.voltage.valueString != ""
                        QGCLabel {
                            text:  qsTr("Voltage")
                            color: qgcPal.colorGrey
                        }
                        QGCLabel {
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           battery ? battery.voltage.valueString + " " + battery.voltage.units : "N/A" 
                        }
                    }

                    ColumnLayout {
                        id:               stateOfChargeContainer
                        Layout.fillWidth: true
                        spacing:          ScreenTools.defaultFontPixelWidth / 6
                        visible:          battery && battery.percentRemaining.valueString != undefined && battery.percentRemaining.valueString != ""
                        QGCLabel {
                            text:  qsTr("SoC")
                            color: qgcPal.colorGrey
                        }
                        QGCLabel {
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           battery ? battery.percentRemaining.valueString + " " + battery.percentRemaining.units : "N/A" 
                        }
                    }
                    ColumnLayout {
                        id: nextThresholdInfo
                        spacing: ScreenTools.defaultFontPixelWidth / 6

                        property string nextThreshold: battery ? battery.nextThresholdName.value : ""
                        property real secondsUntilNextThreshold: battery ? battery.timeUntilNextThreshold.value : -1

                        visible: nextThreshold !== "" && secondsUntilNextThreshold >= 0

                        QGCLabel {
                            Layout.fillWidth: true
                            text:             nextThresholdInfo.nextThreshold
                            color:            qgcPal.colorGrey
                            elide:            Text.ElideRight
                            maximumLineCount: 1
                        }

                        QGCLabel {
                            Layout.fillWidth: true
                            font.pointSize:   ScreenTools.mediumFontPointSize
                            text: {
                                const minutes = Math.floor(nextThresholdInfo.secondsUntilNextThreshold / 60)
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
            id: loader
            onLoaded: {
                batteryPopup.x = -(getWidthOfChildren() + _toolsMargin * 2)
                if (!batteryWidgetContainer.winchControlVisible) {
                    batteryPopup.y = -(getHeightOfChildren() - (batteryWidgetContainer.height - _toolsMargin))
                }
            }
            function getWidthOfChildren() {
                // As loader.item is component with width and height 0, we need to access the child item to get the real width
                if (loader.item && loader.item.children && loader.item.children.length > 0) {
                    return loader.item.children[0].width
                }
                return 0
            }
            function getHeightOfChildren() {
                if (loader.item && loader.item.children && loader.item.children.length > 0) {
                    return loader.item.children[0].height
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