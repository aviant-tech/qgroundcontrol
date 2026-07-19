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
import QtQuick.Effects
import QtLocation
import QtPositioning

import QGroundControl
import QGroundControl.ScreenTools
import QGroundControl.Vehicle
import QGroundControl.Controls
import QGroundControl.ADSBVehicle

/// Marker for displaying a vehicle location on the map
MapQuickItem {
    id: _root

    property var    vehicle                                                         /// Vehicle object, undefined for ADSB vehicle
    property var    map
    property double altitude:       Number.NaN                                      ///< NAN to not show
    property string callsign:       ""                                              ///< Vehicle callsign
    property double heading:        vehicle ? vehicle.heading.value : Number.NaN    ///< Vehicle heading, NAN for none
    property real   size:           ScreenTools.defaultFontPixelHeight * 3          /// Default size for icon, most usage overrides this
    property bool   alert:          false                                           /// Collision alert
    property var    emitterType:    ADSBVehicle.EMITTER_TYPE_NO_INFO                /// ADSB vehicle type
    property int    icaoAddress                                                     /// ICAO address for ADSB vehicle
    property bool   oldSignal:      false                                           /// True if the ADSB signal is old

    anchorPoint.x:  vehicleItem.width  / 2
    anchorPoint.y:  vehicleItem.height / 2
    visible:        coordinate.isValid

    property var    _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool   _adsbVehicle:   vehicle ? false : true
    property var    _map:           map
    property bool   _multiVehicle:  QGroundControl.multiVehicleManager.vehicles.count > 1
    property var    _adsbVehicleManager: QGroundControl.adsbVehicleManager

    sourceItem: Item {
        id:         vehicleItem
        width:      vehicleIcon.width
        height:     vehicleIcon.height
        opacity:    _adsbVehicle ? (oldSignal ? 0.4 : 1.0) : (vehicle === _activeVehicle ? 1.0 : 0.5)

        MultiEffect {
            source: vehicleIcon
            shadowEnabled: vehicleIcon.visible && _adsbVehicle
            shadowColor: Qt.rgba(0.94,0.91,0,1.0)
            shadowVerticalOffset: 4
            shadowHorizontalOffset: 4
            shadowBlur: 1.0
            shadowOpacity: 0.5
            shadowScale: 1.3
            blurMax: 32
            blurMultiplier: .1
        }
            
        Repeater {
            model: vehicle ? vehicle.gimbalController.gimbals : [] 
            
            Item {
                id:                           canvasItem
                anchors.centerIn:             vehicleItem
                width:                        vehicleItem.width * 2
                height:                       vehicleItem.height * 2
                property var gimbalYaw:       object.absoluteYaw.rawValue
                rotation:                     gimbalYaw + 180
                onGimbalYawChanged:           canvas.requestPaint()
                visible:                      vehicle && !isNaN(gimbalYaw) && QGroundControl.settingsManager.gimbalControllerSettings.showAzimuthIndicatorOnMap.rawValue
                opacity:                      object === vehicle.gimbalController.activeGimbal ? 1.0 : 0.4

                Canvas {
                    id:                           canvas
                    anchors.centerIn:             canvasItem
                    anchors.verticalCenterOffset: vehicleItem.width
                    width:                        vehicleItem.width
                    height:                       vehicleItem.height

                    onPaint:                      paintHeading()

                    function paintHeading() {
                        var context = getContext("2d")
                        // console.log("painting heading " + object.param1Raw + " " + opacity + " " + visible + " " + _index)
                        context.clearRect(0, 0, vehicleIcon.width, vehicleIcon.height);

                        var centerX = canvas.width / 2;
                        var centerY = canvas.height / 2;
                        var length = canvas.height * 1.3
                        var width = canvas.width * 0.6

                        var point1 = [centerX - width , centerY + canvas.height * 0.6]
                        var point2 = [centerX, centerY - canvas.height * 0.5]
                        var point3 = [centerX + width , centerY + canvas.height * 0.6]
                        var point4 = [centerX, centerY + canvas.height * 0.2]

                        // Draw the arrow
                        context.save();
                        context.globalAlpha = 0.9;
                        context.beginPath();
                        context.moveTo(centerX, centerY + canvas.height * 0.2);
                        context.lineTo(point1[0], point1[1]);
                        context.lineTo(point2[0], point2[1]);
                        context.lineTo(point3[0], point3[1]);
                        context.lineTo(point4[0], point4[1]);
                        context.closePath();

                        const gradient = context.createLinearGradient(canvas.width / 2, canvas.height , canvas.width / 2, 0);
                        gradient.addColorStop(0.3, Qt.rgba(255,255,255,0));
                        gradient.addColorStop(0.5, Qt.rgba(255,255,255,0.5));
                        gradient.addColorStop(1, qgcPal.mapIndicator);

                        context.fillStyle = gradient;
                        context.fill();
                        context.restore();
                    }
                }
            }
        }

        Image {
            id:                 vehicleIcon
            source:             _adsbVehicle ? getAdsbIcon(emitterType, alert) : vehicle.vehicleImageOpaque
            mipmap:             true
            width:              _root.size
            sourceSize.width:   _root.size
            fillMode:           Image.PreserveAspectFit
            transform: Rotation {
                origin.x:       vehicleIcon.width  / 2
                origin.y:       vehicleIcon.height / 2
                angle:          isNaN(heading) ? 0 : heading
            }

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
                    default:
                        return alert ? "/qmlimages/AlertUnknown.svg" : "/qmlimages/AwarenessUnknown.svg"
                }
            }

            MouseArea {
                anchors.fill:   parent
                enabled:        _adsbVehicle
                onClicked: {
                    if (_adsbVehicle) {
                        mainWindow.showMessageDialog(
                            qsTr("Hide vehicle %1?").arg(callsign),
                            qsTr("You can unhide it in the toolbar."),
                            Dialog.Yes | Dialog.No,
                            function() {
                                if (_adsbVehicleManager && icaoAddress) {
                                    _adsbVehicleManager.setHiddenForADSBVehicle(Number(icaoAddress), true)
                                }
                            })
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
                            var altitudeValueString = QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(altitudeDifference).toFixed(0)
                            var altitudeUnit = QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString
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
}
