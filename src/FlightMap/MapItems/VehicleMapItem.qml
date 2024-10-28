/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick              2.3
import QtQuick.Controls     1.2
import QtQuick.Dialogs      1.2
import QtQuick.Layouts      1.3
import QtLocation           5.3
import QtPositioning        5.3
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ADSBVehicle   1.0

/// Marker for displaying a vehicle location on the map
MapQuickItem {
    property var    vehicle                                                         /// Vehicle object, undefined for ADSB vehicle
    property var    map
    property double altitude:       Number.NaN                                      ///< NAN to not show
    property string callsign:       ""                                              ///< Vehicle callsign
    property double heading:        vehicle ? vehicle.heading.value : Number.NaN    ///< Vehicle heading, NAN for none
    property real   size:           _adsbVehicle ? _adsbSize : _uavSize             /// Size for icon
    property bool   alert:          false                                           /// Collision alert
    property var    emitterType:    ADSBVehicle.EMITTER_TYPE_NO_INFO
    property int    icaoAddress                                                     /// ICAO address for ADSB vehicle
    property var    adsbVehicleManager: QGroundControl.adsbVehicleManager

    anchorPoint.x:  vehicleItem.width  / 2
    anchorPoint.y:  vehicleItem.height / 2
    visible:        coordinate.isValid

    property var    _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool   _adsbVehicle:   vehicle ? false : true
    property real   _uavSize:       ScreenTools.defaultFontPixelHeight * 5
    property real   _adsbSize:      ScreenTools.defaultFontPixelHeight * 2.5
    property var    _map:           map
    property bool   _multiVehicle:  QGroundControl.multiVehicleManager.vehicles.count > 1
    
    property var hideVehicleDialog

    Component.onDestruction: {
        if (hideVehicleDialog && hideVehicleDialog.opened) {
            hideVehicleDialog.close()
        }

    }

    sourceItem: Item {
        id:         vehicleItem
        width:      vehicleIcon.width
        height:     vehicleIcon.height
        opacity:    _adsbVehicle || vehicle === _activeVehicle ? 1.0 : 0.5

        Rectangle {
            id:                 vehicleShadow
            anchors.fill:       vehicleIcon
            color:              Qt.rgba(1,1,1,1)
            radius:             width * 0.5
            visible:            false
        }
        DropShadow {
            anchors.fill:       vehicleShadow
            visible:            vehicleIcon.visible && _adsbVehicle
            horizontalOffset:   4
            verticalOffset:     4
            radius:             32.0
            samples:            65
            color:              Qt.rgba(0.94,0.91,0,0.5)
            source:             vehicleShadow
        }

        Image {
            function getAdsbIcon(emitterType, alert) {
                switch (emitterType) {
                    case ADSBVehicle.EMITTER_TYPE_LIGHT:
                    case ADSBVehicle.EMITTER_TYPE_SMALL:
                    case ADSBVehicle.EMITTER_TYPE_LARGE:
                    case ADSBVehicle.EMITTER_TYPE_HEAVY:
                    case ADSBVehicle.EMITTER_TYPE_HIGHLY_MANUV:
                    case ADSBVehicle.EMITTER_TYPE_GLIDER:
                    case ADSBVehicle.EMITTER_TYPE_ULTRA_LIGHT:
                        return alert ? "/qmlimages/AlertAircraft.svg" : "/qmlimages/AwarenessAircraft.svg"
                    case ADSBVehicle.EMITTER_TYPE_UAV:
                        return alert ? "/qmlimages/AlertDrone.svg" : "/qmlimages/AwarenessDrone.svg"
                    case ADSBVehicle.EMITTER_TYPE_ROTOCRAFT:
                        return alert ? "/qmlimages/AlertHeli.svg" : "/qmlimages/AwarenessHeli.svg"
                    case ADSBVehicle.EMITTER_TYPE_PARACHUTE:
                        return alert ? "/qmlimages/AlertPara.svg" : "/qmlimages/AwarenessPara.svg"
                    case ADSBVehicle.EMITTER_TYPE_NO_INFO:
                    case ADSBVehicle.EMITTER_TYPE_HIGH_VORTEX_LARGE:
                    case ADSBVehicle.EMITTER_TYPE_UNASSIGNED:
                    case ADSBVehicle.EMITTER_TYPE_LIGHTER_AIR:
                    case ADSBVehicle.EMITTER_TYPE_UNASSIGNED2:
                    case ADSBVehicle.EMITTER_TYPE_SPACE:
                    case ADSBVehicle.EMITTER_TYPE_UNASSGINED3:
                    case ADSBVehicle.EMITTER_TYPE_EMERGENCY_SURFACE:
                    case ADSBVehicle.EMITTER_TYPE_SERVICE_SURFACE:
                    case ADSBVehicle.EMITTER_TYPE_POINT_OBSTACLE:
                    default:
                        return alert ? "/qmlimages/AlertUnknown.svg" : "/qmlimages/AwarenessUnknown.svg"
                }
            }

            id:                 vehicleIcon
            source:             _adsbVehicle ? getAdsbIcon(emitterType, alert) : vehicle.vehicleImageOpaque
            mipmap:             true
            width:              size
            sourceSize.width:   size
            fillMode:           Image.PreserveAspectFit
            transform: Rotation {
                origin.x:       vehicleIcon.width  / 2
                origin.y:       vehicleIcon.height / 2
                angle:          isNaN(heading) ? 0 : heading
            }

            MouseArea {
                anchors.fill: parent
                enabled:      _adsbVehicle
                onClicked: {
                    if (_adsbVehicle) {
                        hideVehicleDialog = mainWindow.showPopupDialogFromComponent(hideVehiclePopup, {callsign: callsign})
                    }
                }
            }
        }

        QGCMapLabel {
            id:                         vehicleLabel
            anchors.top:                parent.bottom
            anchors.horizontalCenter:   parent.horizontalCenter
            map:                        _map
            text:                       vehicleLabelText
            font.pointSize:             _adsbVehicle ? ScreenTools.defaultFontPointSize : ScreenTools.smallFontPointSize
            visible:                    _adsbVehicle ? !isNaN(altitude) : _multiVehicle
            property string vehicleLabelText: {
                if (!visible) return ""

                var label = ""
                if (_adsbVehicle && _activeVehicle) {
                    label += callsign
                    if (coordinate.isValid && _activeVehicle.coordinate.isValid) {
                        var distance = coordinate.distanceTo(_activeVehicle.coordinate)
                        var distanceValueString = QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(distance).toFixed(0)
                        var distanceUnit = QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
                        label += "\nd+" + distanceValueString + " " + distanceUnit
        
                        if (!isNaN(altitude)) {
                            var altitudeDifference = altitude - _activeVehicle.coordinate.altitude
                            var altitudePrefix = altitudeDifference > 0 ? "+" : ""
                            var altitudeValueString = QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(altitudeDifference).toFixed(0)
                            var altitudeUnit = QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
                            label += "\na" + altitudePrefix + altitudeValueString + " " + altitudeUnit
                        }
                    }
                } else if (_multiVehicle) {
                    label += vehicle.name
                }
                return label
            }
        }
    }

    Component {
        id: hideVehiclePopup
        QGCPopupDialog {
            title: qsTr("Hide vehicle " + callsign + "?")
            buttons:    StandardButton.Yes | StandardButton.No

            QGCLabel { text: qsTr("You can unhide it in the toolbar") }

            function accept() {
                if (adsbVehicleManager && icaoAddress) {
                    adsbVehicleManager.setHiddenForADSBVehicle(Number(icaoAddress), true)
                } else {
                    mainWindow.showMessageDialog(qsTr("Error"), qsTr("ADSB vehicle manager or icao address not found"))
                    console.log("ADSB vehicle manager or icao address not found")
                }                
                hideDialog()
            }
            function reject() {
                hideDialog()
            }
        }
    }
}
