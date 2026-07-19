/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette

//-------------------------------------------------------------------------
//-- Telemetry RSSI
Item {
    id:             control
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          telemIcon.width * 1.1

    property bool showIndicator: _hasTelemetry

    property var  _activeVehicle:   QGroundControl.multiVehicleManager.activeVehicle
    property bool _hasTelemetry:    _activeVehicle.telemetryLRSSI !== 0

    // Aviant (A12): colour the RSSI icon by receive strength. Thresholds were the AviantSettings
    // facts rssiWarning/rssiAlert in 4.2; that settings group is not yet ported to v5, so the
    // 4.2 defaults are applied here as constants.
    property real _rssiWarning:     -90     // dBm, below this -> warning (orange)
    property real _rssiAlert:       -105    // dBm, below this -> alert (flashing red)
    property bool _pulser:          false   // Switches on/off at 1Hz, used to flash the rssi icon on alert

    function linkColor() {
        if (!_activeVehicle || _activeVehicle.telemetryLRSSI > -2) {
            // -1 is used for invalid/missing data, and positive numbers are not expected/valid here,
            // so for these numbers we use the default colour.
            return qgcPal.buttonText
        } else if (_activeVehicle.telemetryLRSSI > _rssiWarning) {
            return qgcPal.colorGreen
        } else if (_activeVehicle.telemetryLRSSI > _rssiAlert) {
            return qgcPal.colorOrange
        } else {
            return _pulser ? qgcPal.colorRed : qgcPal.buttonText
        }
    }

    Timer {
        interval: 500; running: true; repeat: true
        onTriggered: _pulser = !_pulser
    }

    QGCColoredImage {
        id:                 telemIcon
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        width:              height
        sourceSize.height:  height
        source:             "/qmlimages/TelemRSSI.svg"
        fillMode:           Image.PreserveAspectFit
        color:              linkColor()
    }

    MouseArea {
        anchors.fill:   parent
        onClicked:      mainWindow.showIndicatorDrawer(telemRSSIInfoPage, control)
    }

    Component {
        id: telemRSSIInfoPage

        ToolIndicatorPage {
            showExpand: false

            contentComponent: SettingsGroupLayout {
                heading: qsTr("Telemetry RSSI Status")

                LabelledLabel {
                    label:      qsTr("Local RSSI:")
                    labelText:  _activeVehicle.telemetryLRSSI + " " + qsTr("dBm")
                }

                LabelledLabel {
                    label:      qsTr("Remote RSSI:")
                    labelText:  _activeVehicle.telemetryRRSSI + " " + qsTr("dBm")
                }

                LabelledLabel {
                    label:      qsTr("RX Errors:")
                    labelText:  _activeVehicle.telemetryRXErrors
                }

                LabelledLabel {
                    label:      qsTr("Errors Fixed:")
                    labelText:  _activeVehicle.telemetryFixed
                }

                LabelledLabel {
                    label:      qsTr("TX Buffer:")
                    labelText:  _activeVehicle.telemetryTXBuffer
                }

                LabelledLabel {
                    label:      qsTr("Local Noise:")
                    labelText:  _activeVehicle.telemetryLNoise
                }

                LabelledLabel {
                    label:      qsTr("Remote Noise:")
                    labelText:  _activeVehicle.telemetryRNoise
                }
            }
        }
    }
}
