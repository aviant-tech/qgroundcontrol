/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick  2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0

Rectangle {
    id:         root
    radius:     _toolsMargin / 2
    color:      qgcPal.window
    opacity:    0.9
    clip:       true
    height:     300

    property var  activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property real vehicleAltitude:   activeVehicle ? activeVehicle.altitudeAMSL.rawValue : 0
    property var  missionController: missionController
    property real climbRate:         activeVehicle ? activeVehicle.climbRate.rawValue : 0
    property real centerAltitude:    _calculateCenterAltitude(missionController, vehicleAltitude)
    property real margins:           ScreenTools.defaultFontPixelWidth / 2
    property real scaleHeight:       height - (margins * 2) - ScreenTools.defaultFontPixelHeight
    property real centerY:           (height - ScreenTools.defaultFontPixelHeight) / 2
    property real availableHeight:   300 // Default value, will be overridden by parent
    
    property real minVisibleRangeInMeters:    60 // Default value, will be overridden by parent
    property real metersBetweenLines:         10 // Default value, will be overridden by parent
    property real visibleRange:               _calculateVisibleRange()
    property real pixelsPerMeter:             scaleHeight / visibleRange
    property bool vehicleFlying:              activeVehicle ? activeVehicle.flying : false
    property int  lineCount:                  Math.floor(visibleRange / metersBetweenLines) + 1

    QGCPalette { id: qgcPal }
    
    function mapRelativeToCenterY(relativeAltitude) {
        return centerY - (relativeAltitude * pixelsPerMeter)
    }

    function _calculateCenterAltitude(controller, vehicleAlt) {
        // Calculates the center altitude to be displayed by the widget
        if (controller.visualItems && controller.currentMissionIndex >= 0) {
            var item = controller.visualItems.get(controller.currentMissionIndex)
            if (item && item.amslEntryAlt !== undefined && !isNaN(item.amslEntryAlt)) {
                return item.amslEntryAlt
            }
        }
        // When not flying, use vehicle altitude as reference point
        return vehicleAlt
    }

    function _calculateVisibleRange() {
        // Calculates the visible range to be displayed by the widget
        if (!activeVehicle || !vehicleFlying || !centerAltitude) {
            return minVisibleRangeInMeters
        }

        var altitudeDifference = Math.abs(vehicleAltitude - centerAltitude)

        // Calculate the number of steps needed based on minVisibleRangeInMeters
        var steps = Math.ceil(altitudeDifference * 2 / minVisibleRangeInMeters)
        var roundedRange = steps * minVisibleRangeInMeters

        return Math.max(minVisibleRangeInMeters, roundedRange)
    }

    Connections {
        target: missionController
        onCurrentMissionIndexChanged: centerAltitude = _calculateCenterAltitude(missionController, vehicleAltitude)
    }

    QGCLabel {
        id:          climbRateLabel
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: (climbRate >= 0 ? "+" : "-") + Math.abs(climbRate).toFixed(1) + " m/s"
        visible: vehicleFlying
        color: "white"
    }

    RowLayout {
        anchors.top: vehicleFlying ? climbRateLabel.bottom : parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: margins
        spacing: margins

        Item {
            id: labelsColumn
            Layout.preferredWidth: maxLabelWidth
            Layout.fillHeight: true
            
            property real maxLabelWidth: 0
            
            Repeater {
                model: lineCount
                
                QGCLabel {
                    id: altLabel
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: Math.round(centerAltitude + ((visibleRange/2) - index * metersBetweenLines)) + "m"
                    color: "white"
                    anchors.right: parent.right
                    y: mapRelativeToCenterY((visibleRange/2) - index * metersBetweenLines) - height/2
                    
                    Component.onCompleted: {
                        labelsColumn.maxLabelWidth = Math.max(labelsColumn.maxLabelWidth, contentWidth)
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Repeater {
                model: lineCount
                
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 1
                    y: mapRelativeToCenterY((visibleRange / 2) - index * metersBetweenLines)
                    color: "#80ffffff"
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 2
                y: centerY
                color: "#be781c"
            }

            Rectangle {
                id: altitudeIndicator
                width: 12
                height: width
                radius: width/2
                color: "white"
                y: mapRelativeToCenterY(vehicleAltitude - centerAltitude) - height/2
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}

