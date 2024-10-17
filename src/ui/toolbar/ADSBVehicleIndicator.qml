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
//-- ADSB Vehicle
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          adsbIcon.width * 1.1

    property var  adsbManager:   QGroundControl.adsbVehicleManager
    property bool showIndicator: adsbManager && adsbManager.hasHiddenADSBVehicle

    QGCColoredImage {
        id:                 adsbIcon
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        width:              height
        sourceSize.height:  height
        source:             "/qmlimages/AwarenessDrone.svg"
        fillMode:           Image.PreserveAspectFit
        visible:            adsbManager && adsbManager.hasHiddenADSBVehicle
    }

    MouseArea {
        anchors.fill: adsbIcon
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

            Column {
                id:               adsbCol
                spacing:          ScreenTools.defaultFontPixelHeight * 0.5
                width:            Math.max(hiddenVehiclesList.width, adsbLabel.width)
                anchors.margins:  ScreenTools.defaultFontPixelHeight
                anchors.centerIn: parent

                QGCLabel {
                    id:                       adsbLabel
                    text:                     qsTr("Hidden ADS-B Vehicles")
                    font.family:              ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                ListView {
                    id: hiddenVehiclesList
                    anchors.margins:    ScreenTools.defaultFontPixelHeight

                    model: adsbManager.hiddenADSBVehicles
                    delegate: Item {
                        height: ScreenTools.defaultFontPixelHeight * 2

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                adsbManager.unhideADSBVehicle(modelData["icaoAddress"])
                                mainWindow.hidePopup()
                            }
                        }

                        Text {
                            text: {
                                console.log(modelData)
                                if (!modelData || !modelData["callsign"]) {
                                    return qsTr("Unknown Vehicle")
                                }
                                conosle.log(modelData["callsign"])
                                if (modelData["callsign"] === "") {
                                    return qsTr("Vehicle without callsign")
                                }
                                return modelData["callsign"]
                            }
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                        }
                    }
                }
            }
        }
    }
}
