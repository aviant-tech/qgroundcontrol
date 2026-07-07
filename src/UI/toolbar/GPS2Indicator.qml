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
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette

//-------------------------------------------------------------------------
//-- Aviant (A60): secondary GPS indicator
Item {
    id:             control
    width:          gps2IndicatorRow.width
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    // Only show when the vehicle reports a second GPS
    property bool showIndicator: _activeVehicle && _activeVehicle.gps2 && _activeVehicle.gps2.count.value >= 0

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    Row {
        id:             gps2IndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth / 2

        QGCColoredImage {
            id:                 gps2Icon
            width:              height
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            source:             "/qmlimages/Gps.svg"
            fillMode:           Image.PreserveAspectFit
            sourceSize.height:  height
            opacity:            (_activeVehicle && _activeVehicle.gps2.count.value >= 0) ? 1 : 0.5
            color:              qgcPal.buttonText
        }

        Column {
            id:                     gps2ValuesColumn
            anchors.verticalCenter: parent.verticalCenter
            visible:                _activeVehicle && _activeVehicle.gps2 && _activeVehicle.gps2.count.valueString !== ""
            spacing:                0

            QGCLabel {
                anchors.horizontalCenter:   gps2Lock.horizontalCenter
                color:                      qgcPal.buttonText
                text:                       _activeVehicle ? _activeVehicle.gps2.count.valueString : ""
            }

            QGCLabel {
                id:         gps2Lock
                color:      getLockColor()
                text:       getLockText()

                function getLockColor() {
                    if (!_activeVehicle || !_activeVehicle.gps2.lock) return qgcPal.buttonText
                    const lockString = _activeVehicle.gps2.lock.enumStringValue
                    if (lockString.includes("RTK")) return qgcPal.colorGreen
                    if (lockString.includes("3D")) return qgcPal.colorOrange
                    if (lockString.includes("2D") || lockString.includes("Static")) return qgcPal.colorBlue
                    if (lockString.includes("None")) return qgcPal.colorRed
                    return qgcPal.buttonText
                }

                function getLockText() {
                    if (!_activeVehicle || !_activeVehicle.gps2.lock) return ""
                    const lockString = _activeVehicle.gps2.lock.enumStringValue
                    if (lockString.includes("RTK")) return "RTK"
                    if (lockString.includes("3D")) return "Lock"
                    if (lockString.includes("2D") || lockString.includes("Static")) return "Other"
                    if (lockString.includes("None")) return "None"
                    return lockString
                }
            }
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked:      mainWindow.showIndicatorDrawer(gps2IndicatorPage, control)
    }

    Component {
        id: gps2IndicatorPage

        ToolIndicatorPage {
            showExpand: false

            contentComponent: SettingsGroupLayout {
                heading: qsTr("Vehicle GPS 2 Status")

                LabelledLabel {
                    label:      qsTr("Satellites")
                    labelText:  (_activeVehicle && _activeVehicle.gps2.count.value >= 0) ? _activeVehicle.gps2.count.valueString : qsTr("N/A", "No data to display")
                }

                LabelledLabel {
                    label:      qsTr("GPS Lock")
                    labelText:  _activeVehicle ? _activeVehicle.gps2.lock.enumStringValue : qsTr("N/A", "No data to display")
                }

                LabelledLabel {
                    label:      qsTr("HDOP")
                    labelText:  _activeVehicle ? _activeVehicle.gps2.hdop.valueString : qsTr("--.--", "No data to display")
                }

                LabelledLabel {
                    label:      qsTr("VDOP")
                    labelText:  _activeVehicle ? _activeVehicle.gps2.vdop.valueString : qsTr("--.--", "No data to display")
                }

                LabelledLabel {
                    label:      qsTr("Course Over Ground")
                    labelText:  _activeVehicle ? _activeVehicle.gps2.courseOverGround.valueString : qsTr("--.--", "No data to display")
                }
            }
        }
    }
}
