/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick                  2.3
import QtQuick.Controls         1.2
import QtQuick.Controls.Styles  1.4
import QtQuick.Layouts          1.2
import QtGraphicalEffects       1.0

import QGroundControl               1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0

SetupPage {
    id:             safetyPage
    pageComponent:  pageComponent
    Component {
        id: pageComponent

        Item {
            width:      Math.max(availableWidth, outerColumn.width)
            height:     outerColumn.height

            FactPanelController {
                id:         controller
            }

            property real _margins:         ScreenTools.defaultFontPixelHeight
            property real _labelWidth:      ScreenTools.defaultFontPixelWidth  * 30
            property real _editFieldWidth:  ScreenTools.defaultFontPixelWidth  * 24
            property real _imageHeight:     ScreenTools.defaultFontPixelHeight * 3
            property real _imageWidth:      _imageHeight * 2

            property Fact _fenceAction:         controller.getParameterFact(-1, "GF_ACTION")
            property Fact _fenceRadius:         controller.getParameterFact(-1, "GF_MAX_HOR_DIST")
            property Fact _fenceAlt:            controller.getParameterFact(-1, "GF_MAX_VER_DIST")
            property Fact _lowBattAction:       controller.getParameterFact(-1, "COM_LOW_BAT_ACT")
            property Fact _rcInMode:            controller.getParameterFact(-1, "COM_RC_IN_MODE")
            property Fact _rcLossAction:        controller.getParameterFact(-1, "NAV_RCL_ACT")
            property Fact _dlLossAction:        controller.getParameterFact(-1, "NAV_DLL_ACT")
            property Fact _disarmLandDelay:     controller.getParameterFact(-1, "COM_DISARM_LAND")
            property Fact _landSpeedMC:         controller.getParameterFact(-1, "MPC_LAND_SPEED", false)
            property Fact _parachuteRequired:   controller.getParameterFact(-1, "COM_PARACHUTE")

            ColumnLayout {
                id:         outerColumn
                spacing:    _margins
                anchors.horizontalCenter:   parent.horizontalCenter

                QGCLabel {
                    text:                   qsTr("Low Battery Failsafe Trigger")
                }

                Rectangle {
                    width:                  mainRow.width  + (_margins * 2)
                    height:                 mainRow.height + (_margins * 2)
                    color:                  qgcPal.windowShade
                    Row {
                        id:                 mainRow
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            Image {
                                mipmap:             true
                                fillMode:           Image.PreserveAspectFit
                                source:             qgcPal.globalTheme === QGCPalette.Light ? "/qmlimages/LowBatteryLight.svg" : "/qmlimages/LowBattery.svg"
                                height:             _imageHeight
                                anchors.centerIn:   parent
                            }
                        }
                        GridLayout {
                            columns:                2
                            anchors.verticalCenter: parent.verticalCenter

                            QGCLabel {
                                text:               qsTr("Failsafe Action:")
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactComboBox {
                                fact:               _lowBattAction
                                indexModel:         false
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }

                            QGCLabel {
                                text:               qsTr("Battery Warn Level:")
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               controller.getParameterFact(-1, "BAT_LOW_THR")
                                Layout.fillWidth:   true
                            }

                            QGCLabel {
                                text:               qsTr("Battery Failsafe Level:")
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               controller.getParameterFact(-1, "BAT_CRIT_THR")
                                Layout.fillWidth:   true
                            }

                            QGCLabel {
                                text:               qsTr("Battery Emergency Level:")
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               controller.getParameterFact(-1, "BAT_EMERGEN_THR")
                                Layout.fillWidth:   true
                            }
                        }
                    }
                }

                QGCLabel {
                    text:                   qsTr("RC Loss Failsafe Trigger")
                }

                Rectangle {
                    width:                  mainRow.width     + (_margins * 2)
                    height:                 rcLossGrid.height + (_margins * 2)
                    color:                  qgcPal.windowShade
                    Row {
                        id:                 rcLossGrid
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            Image {
                                mipmap:             true
                                fillMode:           Image.PreserveAspectFit
                                source:             qgcPal.globalTheme === QGCPalette.Light ? "/qmlimages/RCLossLight.svg" : "/qmlimages/RCLoss.svg"
                                height:             _imageHeight
                                anchors.centerIn:   parent
                            }
                        }
                        GridLayout {
                            columns:                2
                            anchors.verticalCenter: parent.verticalCenter

                            QGCLabel {
                                text:               qsTr("RC Control Input Mode:")
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactComboBox {
                                fact:               _rcInMode
                                indexModel:         false
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }

                            QGCLabel {
                                text:               qsTr("Failsafe Action:")
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactComboBox {
                                fact:               _rcLossAction
                                indexModel:         false
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }

                            QGCLabel {
                                text:               qsTr("RC Loss Timeout:")
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               controller.getParameterFact(-1, "COM_RC_LOSS_T")
                                Layout.fillWidth:   true
                            }
                        }
                    }
                }

                QGCLabel {
                    text:                   qsTr("Data Link Loss Failsafe Trigger")
                }

                Rectangle {
                    width:                  mainRow.width           + (_margins * 2)
                    height:                 dataLinkLossGrid.height + (_margins * 2)
                    color:                  qgcPal.windowShade
                    Row {
                        id:                 dataLinkLossGrid
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            Image {
                                mipmap:             true
                                fillMode:           Image.PreserveAspectFit
                                source:             qgcPal.globalTheme === QGCPalette.Light ? "/qmlimages/DatalinkLossLight.svg" : "/qmlimages/DatalinkLoss.svg"
                                height:             _imageHeight
                                anchors.centerIn:   parent
                            }
                        }
                        GridLayout {
                            columns:                2
                            anchors.verticalCenter: parent.verticalCenter

                            QGCLabel {
                                text:               qsTr("Failsafe Action:")
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactComboBox {
                                fact:               _dlLossAction
                                indexModel:         false
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }

                            QGCLabel {
                                text:               qsTr("Data Link Loss Timeout:")
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               controller.getParameterFact(-1, "COM_DL_LOSS_T")
                                Layout.fillWidth:   true
                            }
                        }
                    }
                }

                QGCLabel {
                    text:                   qsTr("Geofence Failsafe Trigger")
                }

                Rectangle {
                    width:                  mainRow.width       + (_margins * 2)
                    height:                 geoFenceGrid.height + (_margins * 2)
                    color:                  qgcPal.windowShade
                    Row {
                        id:                 geoFenceGrid
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            Image {
                                mipmap:             true
                                fillMode:           Image.PreserveAspectFit
                                source:             qgcPal.globalTheme === QGCPalette.Light ? "/qmlimages/GeoFenceLight.svg" : "/qmlimages/GeoFence.svg"
                                height:             _imageHeight
                                anchors.centerIn:   parent
                            }
                        }

                        GridLayout {
                            columns:                2
                            anchors.verticalCenter: parent.verticalCenter

                            QGCLabel {
                                text:               qsTr("Action on breach:")
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactComboBox {
                                fact:               _fenceAction
                                indexModel:         false
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }

                            QGCCheckBox {
                                id:                 fenceRadiusCheckBox
                                text:               qsTr("Max Radius:")
                                checked:            _fenceRadius.value > 0
                                onClicked:          _fenceRadius.value = checked ? 100 : 0
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               _fenceRadius
                                enabled:            fenceRadiusCheckBox.checked
                                Layout.fillWidth:   true
                            }

                            QGCCheckBox {
                                id:                 fenceAltMaxCheckBox
                                text:               qsTr("Max Altitude:")
                                checked:            _fenceAlt ? _fenceAlt.value > 0 : false
                                onClicked:          _fenceAlt.value = checked ? 100 : 0
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               _fenceAlt
                                enabled:            fenceAltMaxCheckBox.checked
                                Layout.fillWidth:   true
                            }
                        }
                    }
                }

                QGCLabel {
                    text:               qsTr("Return To Launch Settings")
                }

                Rectangle {
                    width:              mainRow.width         + (_margins * 2)
                    height:             returnHomeGrid.height + (_margins * 2)
                    color:              qgcPal.windowShade
                    Row {
                        id:                 returnHomeGrid
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            QGCColoredImage {
                                color:              qgcPal.text
                                source:             controller.vehicle.fixedWing ? "/qmlimages/ReturnToHomeAltitude.svg" : "/qmlimages/ReturnToHomeAltitudeCopter.svg"
                                height:             _imageHeight
                                width:              _imageHeight * 2
                                anchors.centerIn:   parent
                            }
                        }
                        GridLayout {
                            columns:                    2
                            anchors.verticalCenter:     parent.verticalCenter

                            QGCLabel {
                                text:                   qsTr("Climb to altitude of:")
                                Layout.minimumWidth:    _labelWidth
                                Layout.fillWidth:       true
                            }
                            FactTextField {
                                fact:                   controller.getParameterFact(-1, "RTL_RETURN_ALT")
                                Layout.minimumWidth:    _editFieldWidth
                                Layout.fillWidth:       true
                            }

                            QGCLabel {
                                text:                   qsTr("Loiter Time")
                                Layout.fillWidth:       true
                            }
                            FactTextField {
                                fact:                   controller.getParameterFact(-1, "RTL_LAND_DELAY")
                                Layout.fillWidth:       true
                            }

                            QGCLabel {
                                text:                   qsTr("Loiter Altitude")
                                Layout.fillWidth:       true
                            }
                            FactTextField {
                                fact:                   controller.getParameterFact(-1, "RTL_DESCEND_ALT")
                                Layout.fillWidth:       true
                            }
                        }
                    }
                }

                QGCLabel {
                    text:               qsTr("Land Mode Settings")
                }

                Rectangle {
                    width:              mainRow.width       + (_margins * 2)
                    height:             landModeGrid.height + (_margins * 2)
                    color:              qgcPal.windowShade
                    Row {
                        id:                 landModeGrid
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            QGCColoredImage {
                                color:              qgcPal.text
                                source:             controller.vehicle.fixedWing ? "/qmlimages/LandMode.svg" : "/qmlimages/LandModeCopter.svg"
                                height:             _imageHeight
                                width:              _imageHeight
                                anchors.centerIn:   parent
                            }
                        }
                        GridLayout {
                            columns:                2
                            anchors.verticalCenter: parent.verticalCenter

                            QGCLabel {
                                id:                 landVelocityLabel
                                text:               qsTr("Landing Descent Rate:")
                                visible:            controller.vehicle && !controller.vehicle.fixedWing
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               _landSpeedMC
                                visible:            controller.vehicle && !controller.vehicle.fixedWing
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }

                            QGCCheckBox {
                                id:                 disarmDelayCheckBox
                                text:               qsTr("Disarm After:")
                                checked:            _disarmLandDelay.value > 0
                                onClicked:          _disarmLandDelay.value = checked ? 2 : 0
                                Layout.fillWidth:   true
                            }
                            FactTextField {
                                fact:               _disarmLandDelay
                                enabled:            disarmDelayCheckBox.checked
                                Layout.fillWidth:   true
                            }
                        }
                    }
                }
                
                QGCLabel {
                    text:               qsTr("Parachute Settings")
                }
                
                Rectangle {
                    width:              mainRow.width        + (_margins * 2)
                    height:             parachuteGrid.height + (_margins * 2)
                    color:              qgcPal.windowShade
                    Row {
                        id:                 parachuteGrid
                        spacing:            _margins
                        anchors.centerIn:   parent
                        Item {
                            width:                  _imageWidth
                            height:                 _imageHeight
                            anchors.verticalCenter: parent.verticalCenter
                            QGCColoredImage {
                                color:              qgcPal.text
                                source:             "/qmlimages/Parachute.svg"
                                height:             _imageHeight
                                width:              _imageHeight
                                anchors.centerIn:   parent
                            }
                        }
                        GridLayout {
                            columns:                2
                            anchors.verticalCenter: parent.verticalCenter

                            QGCLabel {
                                id:                 parachuteRequiredLabel
                                text:               qsTr("Parachute Required\nFor Arming:")
                                Layout.minimumWidth:_labelWidth
                                Layout.fillWidth:   true
                            }
                            FactComboBox {
                                fact:               _parachuteRequired
                                indexModel:         false
                                Layout.minimumWidth:_editFieldWidth
                                Layout.fillWidth:   true
                            }
                        }
                    }
                }
            }
        }
    }
}

