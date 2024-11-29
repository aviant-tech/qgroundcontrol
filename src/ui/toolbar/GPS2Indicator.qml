/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.11
import QtQuick.Layouts  1.11

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

//-------------------------------------------------------------------------
//-- GPS Indicator
Item {
    id:             _root
    width:          (gps2ValuesColumn.x + gps2ValuesColumn.width) * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: true

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    Component {
        id: gps2Info

        Rectangle {
            width:  gps2Col.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: gps2Col.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 gps2Col
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(gps2Grid.width, gps2Label.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             gps2Label
                    text:           (_activeVehicle && _activeVehicle.gps2.count.value >= 0) ? qsTr("GPS 2 Status") : qsTr("GPS Data 2 Unavailable")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 gps2Grid
                    visible:            (_activeVehicle && _activeVehicle.gps2.count.value >= 0)
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    anchors.horizontalCenter: parent.horizontalCenter
                    columns: 2

                    QGCLabel { text: qsTr("GPS Count:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps2.count.valueString : qsTr("N/A", "No data to display") }
                    QGCLabel { text: qsTr("GPS Lock:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps2.lock.enumStringValue : qsTr("N/A", "No data to display") }
                    QGCLabel { text: qsTr("HDOP:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps2.hdop.valueString : qsTr("--.--", "No data to display") }
                    QGCLabel { text: qsTr("VDOP:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps2.vdop.valueString : qsTr("--.--", "No data to display") }
                    QGCLabel { text: qsTr("Course Over Ground:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.gps2.courseOverGround.valueString : qsTr("--.--", "No data to display") }
                }
            }
        }
    }

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
        anchors.leftMargin:     ScreenTools.defaultFontPixelWidth / 2
        anchors.left:           gps2Icon.right

        QGCLabel {
            anchors.horizontalCenter:   gps2Lock.horizontalCenter
            visible:                    _activeVehicle && _activeVehicle.gps2.count.valueString !== ""
            color:                      qgcPal.buttonText
            text:                       _activeVehicle ? _activeVehicle.gps2.count.valueString : ""
        }

        QGCLabel {
            id:         gps2Lock
            visible:    _activeVehicle && _activeVehicle.gps2.lock && _activeVehicle.gps2.lock.enumStringValue !== ""
            color:      getLockColor()
            text:       getLockText()

            function getLockColor() {
                if (!_activeVehicle) return qgcPal.buttonText
                const lockString = _activeVehicle.gps2.lock.enumStringValue
                if (lockString.includes("RTK")) return qgcPal.colorGreen
                if (lockString.includes("3D")) return qgcPal.colorOrange
                if (lockString.includes("2D") || lockString.includes("Static")) return qgcPal.colorBlue
                if (lockString.includes("None")) return qgcPal.colorRed
                return qgcPal.buttonText
            }

            function getLockText() {
                // If it contains "RTK", we have RTK lock.
                // If the lock string contains "3D" and not "RTK", we have 3D lock.
                // As a pilot, I don't care if it's RTK fix or RTK Float, so we just show RTK.
                if (!_activeVehicle) return ""
                const lockString = _activeVehicle.gps2.lock.enumStringValue
                if (lockString.includes("RTK")) return "RTK"
                if (lockString.includes("3D")) return "Lock"
                if (lockString.includes("2D") || lockString.includes("Static")) return "Other"
                if (lockString.includes("None")) return "None"
                return lockString
            }
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, gps2Info)
        }
    }
}
