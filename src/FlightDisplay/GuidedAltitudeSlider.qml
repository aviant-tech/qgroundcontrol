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

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Vehicle       1.0

/// Altitude slider for guided change altitude command
Rectangle {
    id:                 _root

    readonly property real _sliderExponent: 3  // Must be an odd integer

    property var  _activeVehicle:       QGroundControl.multiVehicleManager.activeVehicle
    property var  _flyViewSettings:     QGroundControl.settingsManager.flyViewSettings
    property real _vehicleAltitude:     _activeVehicle ? _activeVehicle.altitudeRelative.rawValue : 0
    property real _vehicleAltitudeAMSL: _activeVehicle ? _activeVehicle.altitudeAMSL.rawValue : 0
    property bool _fixedWing:           _activeVehicle ? _activeVehicle.fixedWing : false
    property real _sliderMaxAlt:        _flyViewSettings ? _flyViewSettings.guidedMaximumAltitude.rawValue : 0
    property real _sliderMinAlt:        _flyViewSettings ? _flyViewSettings.guidedMinimumAltitude.rawValue : 0
    property bool _flying:              _activeVehicle ? _activeVehicle.flying : false
    property bool _useAMSL:             _flyViewSettings ? _flyViewSettings.guidedAltitudeAMSL.rawValue : false
    property bool _useDelta:            _flyViewSettings ? _flyViewSettings.guidedAltitudeDeltaMode.rawValue : false
    property real _sliderMaxChange:     _flyViewSettings ? _flyViewSettings.guidedAltitudeMaxChange.rawValue : 0
    property real _distanceToGround:    _activeVehicle ? _activeVehicle.distanceSensors.rotationPitch270.value : NaN
    
    property real   altGainRange:           _useDelta ? _sliderMaxChange : Math.max(_sliderMaxAlt - _vehicleAltitude, 0)
    property real   altLossRange:           _useDelta ? _sliderMaxChange : Math.max(_vehicleAltitude - _sliderMinAlt, 0)

    function reset() {
        altSlider.value = 0
    }

    function setToMinimumTakeoff() {
        altField.setToMinimumTakeoff()
    }

    /// Returns the user specified change in altitude from the current vehicle altitude
    function getAltitudeChangeValue() {
        return altField.newAltitudeMeters - _vehicleAltitude
    }

    function log10(value) {
        if (value === 0) {
            return 0
        } else {
            return Math.log(value) / Math.LN10
        }
    }

    Column {
        id:                 headerColumn
        anchors.margins:    _margins
        anchors.top:        parent.top
        anchors.left:       parent.left
        anchors.right:      parent.right

        QGCLabel {
            anchors.left:           parent.left
            anchors.right:          parent.right
            wrapMode:               Text.WordWrap
            horizontalAlignment:    Text.AlignHCenter
            text:                   _useAMSL ? qsTr("New Alt\nAMSL") : qsTr("New Alt\nRel Home")
            color:                  altField.color
        }

        QGCLabel {
            id:                         altField
            anchors.horizontalCenter:   parent.horizontalCenter
            horizontalAlignment:        Text.AlignHCenter
            text:                       labelText
            color:                      !isNaN(_distanceToGround) && ((-altLossGain > _distanceToGround) || (altLossGain > 120.0 - _distanceToGround)) ? qgcPal.warningText : qgcPal.text

            property real   altExp:                 Math.pow(altSlider.value, _sliderExponent)
            property real   altLossGain:            altExp * (altSlider.value > 0 ? altGainRange : altLossRange)
            property real   newAltitudeMeters:      _vehicleAltitude + altLossGain
            property real   newAltitudeAMSLMeters:  _vehicleAltitudeAMSL + altLossGain
            property string newAltitudeAppUnits:    QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(
                                                        _useAMSL ? newAltitudeAMSLMeters : newAltitudeMeters).toFixed(1)
            // Concatenate new altitude and unit with change relative to current altitude                                            
            property string labelText:              newAltitudeAppUnits + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
                                                        + "\n(" + (altLossGain < 0 ? "" : "+") + QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(altLossGain).toFixed(1)
                                                        + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString + ")"

            function setToMinimumTakeoff() {
                altSlider.value = Math.pow(_activeVehicle.minimumTakeoffAltitude() / altGainRange, 1.0/_sliderExponent)
            }
        }
    }

    QGCSlider {
        id:                 altSlider
        anchors.margins:    _margins
        anchors.top:        headerColumn.bottom
        anchors.bottom:     parent.bottom
        anchors.left:       parent.left
        anchors.right:      parent.right
        orientation:        Qt.Vertical
        minimumValue:       _flying ? -1 : 0
        maximumValue:       1
        zeroCentered:       true
        rotation:           180
        thresholdIndicator:    _flying ? (1.0 - Math.pow(Math.max(0, Math.min(1, _distanceToGround / altLossRange)), 1.0/_sliderExponent)) * 0.5 : NaN

        // We want slide up to be positive values
        transform: Rotation {
            origin.x:   altSlider.width  / 2
            origin.y:   altSlider.height / 2
            angle:      180
        }
    }
}
