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
import QtQuick.Controls 1.2
import QtQuick.Dialogs  1.2

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

//-------------------------------------------------------------------------
//-- ADSB Vehicle
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          (adsbValuesColumn.x + adsbValuesColumn.width) * 1.1

    property var  adsbManager:    QGroundControl.adsbVehicleManager
    property int  vehicleCount:   adsbManager ? adsbManager.adsbVehicles.count : 0
    property bool showIndicator:  true

    // We can see up to 3 of our own indicators (HemsWX drop, self SafeSky, self base stations),
    // so 1-3 is orange (may be only our own) and 4+ is green (at least one external aircraft).
    function statusColor() {
        if (vehicleCount >= 4) return qgcPal.colorGreen
        if (vehicleCount >= 1) return qgcPal.colorOrange
        return qgcPal.colorRed
    }

    Image {
        id:                 adsbIcon
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        width:              height
        sourceSize.height:  height
        source:             "/qmlimages/AwarenessDrone.svg"
        fillMode:           Image.PreserveAspectFit
    }

    Column {
        id:                     adsbValuesColumn
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin:     ScreenTools.defaultFontPixelWidth / 2
        anchors.left:           adsbIcon.right

        QGCLabel {
            color:                  statusColor()
            text:                   vehicleCount.toString()
            font.family:            ScreenTools.demiboldFontFamily
        }
        QGCLabel {
            color:                  statusColor()
            text:                   qsTr("acft")
            font.pointSize:         ScreenTools.smallFontPointSize
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, hiddenVehiclesPopup)
        }
    }

    Component {
        id: hiddenVehiclesPopup

        Rectangle {
            width:        adsbCol.width + ScreenTools.defaultFontPixelWidth  * 3
            height:       adsbCol.height + ScreenTools.defaultFontPixelHeight * 2
            radius:       ScreenTools.defaultFontPixelHeight * 0.5
            color:        qgcPal.window
            border.color: qgcPal.text

            ColumnLayout {
                id:               adsbCol
                spacing:          ScreenTools.defaultFontPixelHeight * 0.5
                anchors.margins:  ScreenTools.defaultFontPixelHeight
                anchors.centerIn: parent

                QGCLabel {
                    id:               adsbLabel
                    text:             qsTr("ADSB Vehicles (%1)").arg(vehicleCount)
                    font.family:      ScreenTools.demiboldFontFamily
                    Layout.alignment: Qt.AlignHCenter
                }

                QGCListView {
                    id:                hiddenVehiclesList
                    anchors.margins:   ScreenTools.defaultFontPixelHeight
                    clip:              true
                    Layout.fillWidth:  true

                    Layout.preferredHeight: Math.min(contentHeight, 300)
                    Layout.maximumHeight:   300

                    model: adsbManager.adsbVehicles

                    delegate: Item {
                        height:          adsbVehicleLabel.implicitHeight
                        width:           hiddenVehiclesList.width
                        anchors.margins: ScreenTools.defaultFontPixelHeight

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (object.hidden) {
                                    if (object.icaoAddress) {
                                        adsbManager.setHiddenForADSBVehicle(Number(object.icaoAddress), false)
                                    }
                                } else {
                                    if (object.icaoAddress) {
                                        adsbManager.setHiddenForADSBVehicle(Number(object.icaoAddress), true)
                                    }
                                }
                            }
                        }

                        QGCLabel {
                            id:    adsbVehicleLabel
                            color: object.hidden ? qgcPal.colorGrey : qgcPal.text
                            text: {
                                if (!object || !object.callsign) {
                                    return qsTr("Unknown Vehicle")
                                }
                                if (object.callsign === "") {
                                    return qsTr("Vehicle without callsign")
                                }
                                return object.callsign
                            }
                        }
                    }
                }

                QGCButton {
                    text:            qsTr("Unhide all")
                    anchors.margins: ScreenTools.defaultFontPixelHeight
                    onClicked: {
                        adsbManager.unhideAllVehicles()
                    }
                }
            }
        }
    }
}
