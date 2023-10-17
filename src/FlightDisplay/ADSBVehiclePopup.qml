import QtQuick                      2.11
import QtQuick.Controls             2.4
import QtQml.Models                 2.1

import QtPositioning 5.15

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ADSBVehicle   1.0

/**
  This is a small overlay view to show ADSB details

  In   FlyViewWidgetLayer.qml

    ADSBVehiclePopup {
        id: adsb_popup
        anchors.bottom: parent.bottom
    }

    The visible is false by default.

    Only one public function:
    function show(is_visible, is_critical, object, own_coord)

    is_visible - if ADSB object is visible on mapControl
    is_critical - if ADSB object is visible within critical ObstacleDistanceOverlay
    object - the ADSB vechile object (ADSBVechile.h)
    own_coord - the current own coordinates

    That information is defined and collected in FlyViewMap.qml, that is the only
    place the information is defined and the implementation signals value from there
    (see signal trafficWarning(bool visible, bool isCritical, variant ifo, variant myCoord))

    It crucial the do the signaling if any of those values changes.

    ADSBVehiclePopup maintains a list of visible objects and shows a list.
    The popup is-a list with following UI function:
    * When a new object is added (is_visible true), the popup is made visible.
    * When the last object is removed (is_visible false) the popup is hidden.
    * When a new object is added, the list focus is on that item.
    * User can scroll left-right to show other items.
    * Long press set focus on the closest object in list.
    * Double-click removes the object from the list.
        (that object wont reappear before is_visible false for it is received)

    Shown information (some more are not possible, this is due spec)
    * Callsign
    * Altitude
    * Distance
    * Type (as defined in ADS-B specs)
    * Illustrial image
    * Indicator if object is within critical distance

    *  All strings can be localized

**/


Item {
    id: _root
    implicitWidth: ScreenTools.defaultFontPixelWidth * 26
    implicitHeight:  ScreenTools.defaultFontPixelHeight * 13
    visible: false

    property var _removed_objects: []
    property var _pos
    property real radius: 4

    QGCPalette { id: qgcPal }


    Rectangle { // background
        anchors.fill: parent
        radius: _root.radius
        color: qgcPal.toolbarBackground
    }

    Timer { // Poll here is far simplex than build up change signaling
        running: _root.visible
        repeat: true
        interval: 1000
        onTriggered: {
            for(let i = 0; i < adsbVechicles.count; ++i) {
                _update(i, adsbVechicles.get(i).adsbVechicle);
            }
        }
    }

    ListModel {
        id: adsbVechicles
        function getByAddress(icaoAddress) {
            for(let i = 0; i < count; ++i) {
                if(icaoAddress === get(i).icaoAddress) {
                    return i;
                }
            }
            return -1;
        }

        function getClosest() {
            let distance = 0xFFFFFFF;
            let closest = 0;
            for(let i = 0; i < count; ++i) {
                let curr = get(i)
                let coord = QtPositioning.coordinate(curr.coord_lat, curr.coord_lon, curr.coord_alt)
                let dist = coord.distanceTo(_root._pos)
                if(dist < distance) {
                    closest = i;
                    distance = dist;
                }
            }
            return closest;
        }
    }

    function _update(index, object) {
        if(!object)
            return;
        adsbVechicles.setProperty(index, "callsign", object.callsign);
        adsbVechicles.setProperty(index, "coord_lat", object.coordinate.latitude) // annoyingly I cannot update the coordinates once (QML bug or just jsonic feature?)
        adsbVechicles.setProperty(index, "coord_lon", object.coordinate.longitude)
        adsbVechicles.setProperty(index, "coord_alt", object.coordinate.altitude)
        adsbVechicles.setProperty(index, "altitude", object.altitude);
        adsbVechicles.setProperty(index, "heading", object.heading);
        adsbVechicles.setProperty(index, "alert", object.alert);
        adsbVechicles.setProperty(index, "type", object.emitter);
    }

    function show(is_visible, is_critical, object, own_coord) {
        //console.log("ADSB update", is_visible, is_critical, object, own_coord);
        if(!object) {
            console.debug("ADSB null object");
            return; // dunno if ever happeds, but better safe than sorry
        }

        // last known pos
        _pos = own_coord

        // do not update removed
        if(_removed_objects.includes(object.icaoAddress)) {
            if(!is_visible) { // if not visible remove from removed
                let index = _removed_objects.indexOf(object.icaoAddress);
                _removed_objects.splice(index, 1)
            }
            return;
        }

        let index = adsbVechicles.getByAddress(object.icaoAddress);
        if(is_visible) {                            // add or update on list
            var info = {
                icaoAddress : object.icaoAddress,
                callsign :  object.callsign,
                coord_lat : object.coordinate.latitude, // must set here as QML does not support location as role
                coord_lon : object.coordinate.longitude, // i.e. geocoordinate erodes to some other type
                coord_alt : object.coordinate.altitude,  // sometimes variantmap, sometimes as a list... :-/
                altitude :object.altitude,
                heading :object.heading,
                alert :object.alert,
                type: object.emitter,
                critical: is_critical,
                adsbVechicle: object

            }
            if(index < 0) {
                adsbVechicles.append(info);
                _root.visible = true;              // set visible
                adsbListView.positionViewAtEnd();
            } else {
                _update(index, object);
                adsbVechicles.setProperty(index, "critical", is_critical);
            }
        } else {                                   // remove from list
            if(index >= 0)
                adsbVechicles.remove(index);
            if(adsbVechicles.count <= 0) {         // hide if nothing to show
                _root.visible = false;
            }
        }
    }

    Component {
        id: adsbItem
        Rectangle {  // make sure image clipping goes right
            radius: _root.radius
            color: "transparent"

            id: item

            width: _root.width
            height: _root.height
            readonly property var coord: QtPositioning.coordinate(coord_lat, coord_lon, coord_alt)

            function get_source(type) {
                switch(type) {
                // This mapping is pretty much random and once there are proper art, can be done with care
                case ADSBVehicle.UAV:
                    return "qrc:/res/adsb/uav.jpg";
                case ADSBVehicle.ROTOCRAFT:
                    return "qrc:/res/adsb/rotocraft.jpg";
                case ADSBVehicle.LIGHTER_AIR:
                    return "qrc:/res/adsb/lighter_air.jpg";
                case ADSBVehicle.PARACHUTE:
                    return "qrc:/res/adsb/parachute.jpg";
                case ADSBVehicle.GLIDER:
                    return "qrc:/res/adsb/glider.jpg";
                case ADSBVehicle.HEAVY:
                case ADSBVehicle.HIGH_VORTEX_LARGE:
                    return "qrc:/res/adsb/big_airplane.jpg";
                case ADSBVehicle.HIGHLY_MANUV:
                case ADSBVehicle.LIGHT:
                    return "qrc:/res/adsb/small_airplane.jpg";
                case ADSBVehicle.SMALL:
                case ADSBVehicle.LARGE:
                    return "qrc:/res/adsb/jet.jpg";
                default:
                    return "qrc:/res/adsb/ufo.jpg";
                }
            }
            Column {
                id: col
                padding: 6
                //not show: QGCLabel {color: qgcPal.text; text: qsTr("icao ") + icaoAddress}
                QGCLabel {color: qgcPal.text; text: qsTr("Callsign: ") + callsign}
                //not show: QGCLabel {color: qgcPal.text; text: qsTr("loc ") + item.coord.toString()}
                QGCLabel {color: qgcPal.text; text: qsTr("Alt: ") + altitude.toFixed(0)}
                QGCLabel {color: qgcPal.text; text: qsTr("Dist: ") + item.coord.distanceTo(_root._pos).toFixed(0)}
                //not show: QGCLabel {color: qgcPal.text; text: qsTr("heading ") + heading}
                //not show: QGCLabel {color: qgcPal.text;  text: qsTr("alert ") + (alert ? qsTr("on") : qsTr("off"))}
                QGCLabel {color: qgcPal.text;  text: qsTr("Type: ") + adsbVechicle.emitterString(type)}
                Image {
                    id: vechicle_image
                    source: get_source(type)
                    fillMode: Image.PreserveAspectFit
                    readonly property double sz: item.width - col.padding * 2
                    width: sz
                    height: sz
                    clip: true
                }
            }
            MouseArea {
                anchors.fill: item
                onDoubleClicked: {
                    let index = adsbVechicles.getByAddress(icaoAddress);
                    _removed_objects.push(icaoAddress)
                    adsbVechicles.remove(index);
                }
                onPressAndHold: {
                    let index = adsbVechicles.getClosest();
                    adsbListView.positionViewAtIndex(index, ListView.Contain);

                }
            }
            Rectangle {
                radius: _root.radius
                anchors.fill: parent
                color: "transparent"
                border.color: critical ? "#80FF0000" : qgcPal.toolbarBackground
                border.width: 4
            }
        }
    }

    QGCListView {
        id: adsbListView
        anchors.fill: parent
        clip: true
        orientation: ListView.Horizontal
        model: adsbVechicles
        delegate: adsbItem
    }
}
