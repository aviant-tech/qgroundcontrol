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

// Used as the base class control for nboth VehicleGPSIndicator and RTKGPSIndicator

Item {
    id:             control
    width:          gpsIndicatorRow.width
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property var    _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool   _rtkConnected:  QGroundControl.gpsRtk.connected.value

    Row {
        id:             gpsIndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth / 2

        Row {
            anchors.top:    parent.top
            anchors.bottom: parent.bottom
            spacing:        -ScreenTools.defaultFontPixelWidth / 2

            QGCLabel {
                id:                     gpsLabel
                rotation:               90
                text:                   qsTr("RTK")
                color:                  qgcPal.buttonText
                anchors.verticalCenter: parent.verticalCenter
                visible:                _rtkConnected
            }

            QGCColoredImage {
                id:                 gpsIcon
                width:              height
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                source:             "/qmlimages/Gps.svg"
                fillMode:           Image.PreserveAspectFit
                sourceSize.height:  height
                opacity:            (_activeVehicle && _activeVehicle.gps.count.value >= 0) ? 1 : 0.5
                color:              qgcPal.buttonText
            }
        }

        Column {
            id:                     gpsValuesColumn
            anchors.verticalCenter: parent.verticalCenter
            visible:                _activeVehicle && _activeVehicle.gps.count.valueString !== ""
            spacing:                0

            QGCLabel {
                anchors.horizontalCenter:   gpsLock.horizontalCenter
                color:              qgcPal.buttonText
                text:               _activeVehicle ? _activeVehicle.gps.count.valueString : ""
            }

            // Aviant (A48/A50/A51): show the GPS lock status (using enumStringValue, not index)
            // coloured green for RTK, orange for 3D, blue for 2D/Static and red for None.
            QGCLabel {
                id:         gpsLock
                color:      getLockColor()
                text:       getLockText()

                function getLockColor() {
                    if (!_activeVehicle || !_activeVehicle.gps.lock) return qgcPal.buttonText
                    const lockString = _activeVehicle.gps.lock.enumStringValue
                    if (lockString.includes("RTK")) return qgcPal.colorGreen
                    if (lockString.includes("3D")) return qgcPal.colorOrange
                    if (lockString.includes("2D") || lockString.includes("Static")) return qgcPal.colorBlue
                    if (lockString.includes("None")) return qgcPal.colorRed
                    return qgcPal.buttonText
                }

                function getLockText() {
                    // As a pilot, I don't care if it's RTK fix or RTK float, so we just show RTK.
                    if (!_activeVehicle || !_activeVehicle.gps.lock) return ""
                    const lockString = _activeVehicle.gps.lock.enumStringValue
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
        onClicked:      mainWindow.showIndicatorDrawer(gpsIndicatorPage, control)
    }

    Component {
        id: gpsIndicatorPage

        GPSIndicatorPage { }
    }
}
