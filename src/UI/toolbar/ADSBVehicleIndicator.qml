/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.MultiVehicleManager
import QGroundControl.ScreenTools
import QGroundControl.Palette

//-------------------------------------------------------------------------
//-- ADSB Vehicle indicator: lets the user unhide hidden ADSB traffic
Item {
    id:             control
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
        anchors.fill:   adsbIcon
        onClicked:      mainWindow.showIndicatorDrawer(hiddenVehiclesDrawer, control)
    }

    Component {
        id: hiddenVehiclesDrawer

        ToolIndicatorPage {
            showExpand: false

            contentComponent: Component {
                ColumnLayout {
                    spacing: ScreenTools.defaultFontPixelHeight * 0.5

                    QGCLabel {
                        text:               qsTr("Unhide hidden ADSB vehicles")
                        font.family:        ScreenTools.demiboldFontFamily
                        Layout.alignment:   Qt.AlignHCenter
                    }

                    QGCListView {
                        id:                     hiddenVehiclesList
                        clip:                   true
                        Layout.fillWidth:       true
                        Layout.preferredHeight: Math.min(contentHeight, ScreenTools.defaultFontPixelHeight * 15)
                        Layout.maximumHeight:   ScreenTools.defaultFontPixelHeight * 15
                        model:                  adsbManager.adsbVehicles

                        delegate: Item {
                            width:      hiddenVehiclesList.width
                            height:     object && object.hidden ? adsbVehicleLabel.implicitHeight : 0
                            visible:    object ? object.hidden : false

                            QGCLabel {
                                id:     adsbVehicleLabel
                                color:  qgcPal.text
                                text: {
                                    if (!object || !object.callsign) return qsTr("Unknown Vehicle")
                                    if (object.callsign === "")      return qsTr("Vehicle without callsign")
                                    return object.callsign
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (object && object.icaoAddress) {
                                        adsbManager.setHiddenForADSBVehicle(Number(object.icaoAddress), false)
                                    }
                                }
                            }
                        }
                    }

                    QGCButton {
                        text:               qsTr("Unhide all")
                        Layout.fillWidth:   true
                        onClicked: {
                            adsbManager.unhideAllVehicles()
                            mainWindow.closeIndicatorDrawer()
                        }
                    }
                }
            }
        }
    }
}
