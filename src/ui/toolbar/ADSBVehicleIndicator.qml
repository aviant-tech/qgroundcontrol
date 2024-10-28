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
    width:          adsbIcon.width * 1.1

    property var  adsbManager:   QGroundControl.adsbVehicleManager
    property bool showIndicator: adsbManager && adsbManager.adsbVehicles.count > 0

    Image {
        id:                 adsbIcon
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        width:              height
        sourceSize.height:  height
        source:             "/qmlimages/AwarenessDrone.svg"
        fillMode:           Image.PreserveAspectFit
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

            ColumnLayout {
                id:               adsbCol
                spacing:          ScreenTools.defaultFontPixelHeight * 0.5
                anchors.margins:  ScreenTools.defaultFontPixelHeight
                anchors.centerIn: parent

                QGCLabel {
                    id:               adsbLabel
                    text:             qsTr("Unhidde hidden ADSB Vehicles")
                    font.family:      ScreenTools.demiboldFontFamily
                    Layout.alignment: Qt.AlignHCenter
                }

                QGCListView {
                    id:                hiddenVehiclesList
                    anchors.margins:   ScreenTools.defaultFontPixelHeight
                    spacing:           ScreenTools.defaultFontPixelHeight / 2
                    clip:              true
                    Layout.fillWidth:  true

                    Layout.preferredHeight: Math.min(contentHeight, 300)
                    Layout.maximumHeight:   300

                    model: adsbManager.adsbVehicles
                    
                    delegate: Item {
                        height:  object.hidden ? adsbVehicleLabel.implicitHeight : 0
                        width:   hiddenVehiclesList.width
                        visible: object.hidden

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                 if (object.icaoAddress) {
                                    adsbManager.setHiddenForADSBVehicle(Number(object.icaoAddress), false)
                                } else {
                                    console.log("ADSB vehicle icao address not found")
                                }
                            }
                        }

                        QGCLabel {
                            id:    adsbVehicleLabel
                            color: qgcPal.text
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
                    text: qsTr("Unhide all")
                    onClicked: {
                        adsbManager.unhideAllVehicles()
                    }
                }
            }
        }
    }
}
