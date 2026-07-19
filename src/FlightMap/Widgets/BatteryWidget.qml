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
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls
import MAVLink

// Aviant (A53/A63): Fly-view battery widget. Shows current/throttle/voltage/consumed-based remaining,
// time-or-mAh until next threshold, a bad-failsafe warning, and a graphical battery bar. Tapping opens
// the detailed BatteryPopup.
Rectangle {
    id:      batteryWidgetContainer
    color:   qgcPal.window
    opacity: 0.9
    radius:  _toolsMargin / 2

    // Overridden by parent
    property int  availableHeight:     600
    property bool winchControlVisible: false

    implicitHeight: Math.min(batteryWidgetColumn.implicitHeight + ScreenTools.defaultFontPixelWidth, availableHeight)

    property var  _activeVehicle:  QGroundControl.multiVehicleManager.activeVehicle
    property var  _aviantSettings: QGroundControl.settingsManager.aviantSettings
    property real _toolsMargin:    ScreenTools.defaultFontPixelWidth * 0.75

    QGCPalette { id: qgcPal }

    ColumnLayout {
        id:              batteryWidgetColumn
        anchors.fill:    parent
        anchors.margins: _toolsMargin

        Repeater {
            model: _activeVehicle ? _activeVehicle.batteries : 0

            Loader {
                Layout.fillWidth:       true
                Layout.preferredHeight: item ? item.implicitHeight : 0
                sourceComponent:        battery ? batteryWidget : null

                property var battery: object
            }
        }
    }

    Component {
        id: batteryWidget

        ColumnLayout {
            spacing:         ScreenTools.defaultFontPixelWidth
            anchors.margins: _toolsMargin
            width:           parent.width

            Rectangle {
                id:               failsafeWarning
                visible:          battery && battery.consumedOffset > _aviantSettings.persistentConsumeWarnLimit.rawValue
                height:           failsafeWarningChild.height + 2 * _toolsMargin
                Layout.fillWidth: true
                radius:           _toolsMargin
                color:            "red"
                QGCLabel {
                    id:                       failsafeWarningChild
                    text:                     "BAD BATTERY FAILSAFES"
                    color:                    "black"
                    font.bold:                true
                    anchors.verticalCenter:   parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

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
                    id:      throttleContainer
                    visible: _activeVehicle != undefined && _activeVehicle.throttlePct.valueString != undefined && _activeVehicle.throttlePct.valueString != ""

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
                id:                bottomContainer
                Layout.fillWidth:  true
                Layout.fillHeight: true

                ColumnLayout {
                    Layout.fillWidth:  true
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
                        visible:          battery && battery.consumedBasedRemaining.valueString != undefined && battery.consumedBasedRemaining.valueString != ""
                        QGCLabel {
                            text:  qsTr("Remaining")
                            color: qgcPal.colorGrey
                        }
                        QGCLabel {
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           battery ? battery.consumedBasedRemaining.valueString + " " + battery.consumedBasedRemaining.units : "N/A"
                        }
                    }
                    ColumnLayout {
                        id:      nextThresholdInfo
                        spacing: ScreenTools.defaultFontPixelWidth / 6

                        property string nextThreshold: battery ? battery.nextThresholdName.value : ""

                        property string unit: _aviantSettings.unitUntilThreshold.enumStringValue
                        property real xUntilNextThreshold: {
                            if (!battery) {
                                return -1
                            } else if (_aviantSettings.unitUntilThreshold.rawValue == 0) {
                                return battery.mahUntilNextThreshold.value
                            } else if (_aviantSettings.unitUntilThreshold.rawValue == 1) {
                                return Math.floor(battery.timeUntilNextThreshold.value / 60)
                            } else {
                                return -1
                            }
                        }

                        visible: nextThreshold !== "" && xUntilNextThreshold >= 0

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
                                return qsTr("%1 %2").arg(nextThresholdInfo.xUntilNextThreshold).arg(nextThresholdInfo.unit)
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true // Makes the battery visual align to the right
                }

                BatteryVisual {
                    batteryItem:         battery
                    Layout.fillHeight:   true
                    Layout.topMargin:    _toolsMargin
                    Layout.bottomMargin: _toolsMargin
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
        id:          batteryPopup
        padding:     ScreenTools.defaultFontPixelWidth * 0.75
        modal:       true
        focus:       true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            width:  loader.width
            height: loader.height
            color:  Qt.rgba(0, 0, 0, 0)
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
                // As loader.item is a component with width and height 0, we need to access the child item to get the real width
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
