/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 1.2
import QtLocation       5.3
import QtPositioning    5.3

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightMap     1.0

/// Rally Point map visuals
Item {
    id: _root
    z: QGroundControl.zOrderMapItems

    property var    map
    property var    myRallyPointController
    property bool   interactive:            false   ///< true: user can interact with items
    property bool   planView:               false   ///< true: visuals showing in plan view

    property bool   _interactive:           interactive
    property var    _rallyPointsComponent
    property bool   _rallyPointsSupported:  myRallyPointController.supported
    property var    _rallyPoints:           myRallyPointController.points

    Component.onCompleted: {
        _rallyPointsComponent = rallyPointsComponent.createObject(map)
    }

    Component.onDestruction: {
        _rallyPointsComponent.destroy()
    }

    Component {
        id: dragAreaComponent

        MissionItemIndicatorDrag {
            mapControl:     _root.map
            itemCoordinate: rallyPointObject ? rallyPointObject.coordinate : null
            visible:        rallyPointObject === myRallyPointController.currentRallyPoint && _root.interactive

            property var rallyPointObject

            onItemCoordinateChanged: {
                if (rallyPointObject) {
                    rallyPointObject.coordinate = itemCoordinate
                }
            }
        }
    }

    Component {
        id: rallyPointComponent

        MapQuickItem {
            id:             itemIndicator
            anchorPoint.x:  sourceItem.anchorPointX
            anchorPoint.y:  sourceItem.anchorPointY
            z:              QGroundControl.zOrderMapItems
            opacity:        _root.opacity

            property var rallyPointObject

            sourceItem: MissionItemIndexLabel {
                id:                 itemIndexLabel
                // Rally point types: 0=Always, 1=MR only, 2=FW only
                label:              rallyPointObject && rallyPointObject.type !== undefined
                                    ? (rallyPointObject.type == 2 ? "F" : rallyPointObject.type == 1 ? "M" : "R")
                                    : "R"
                important:          rallyPointObject ? (rallyPointObject.type == 2) : false
                checked:            rallyPointObject
                                    ? (_editingLayer == _layerRallyPoints ? rallyPointObject === myRallyPointController.currentRallyPoint : false)
                                    : false
                highlightSelected:  true
                onClicked:          if (rallyPointObject) { myRallyPointController.currentRallyPoint = rallyPointObject }
            }
        }
    }

    // Add all rally points to the map
    Component {
        id: rallyPointsComponent

        Repeater {
            model: _rallyPoints

            delegate: Item {
                opacity:        _root.opacity
                property var _visuals: [ ]

                Component.onCompleted: {
                    var rallyPointIndicator = rallyPointComponent.createObject(map)
                    rallyPointIndicator.coordinate = Qt.binding(function() { return object.coordinate })
                    rallyPointIndicator.rallyPointObject = Qt.binding(function() { return object })
                    map.addMapItem(rallyPointIndicator)
                    _visuals.push(rallyPointIndicator)

                    var dragArea = dragAreaComponent.createObject(map, { "itemIndicator": rallyPointIndicator, "rallyPointObject": object })
                    _visuals.push(dragArea)
                }

                Component.onDestruction: {
                    for (var i=0; i<_visuals.length; i++) {
                        _visuals[i].destroy()
                    }
                    _visuals = [ ]
                }
            }
        }
    }

}
