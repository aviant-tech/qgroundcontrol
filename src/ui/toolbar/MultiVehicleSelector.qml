/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick                              2.11
import QtQuick.Controls                     2.4

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

//-------------------------------------------------------------------------
//-- Multiple Vehicle Selector
QGCComboBox {
    anchors.verticalCenter: parent.verticalCenter
    font.pointSize:         ScreenTools.mediumFontPointSize
    currentIndex:           -1
    sizeToContents:         true
    model:                  _vehicleModel

    property bool showIndicator: _multipleVehicles

    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property bool   _multipleVehicles:  QGroundControl.multiVehicleManager.vehicles.count > 0
    property var    _vehicleModel:      [ ]
    property var    _vehicleIds:        [ ]

    Connections {
        target:         QGroundControl.multiVehicleManager.vehicles
        onCountChanged:  _updateVehicleModel()
    }

    Component.onCompleted:      _updateVehicleModel()
    on_ActiveVehicleChanged:    _updateVehicleModel()

    function _updateVehicleModel() {
        var newCurrentIndex = -1
        var newModel = [ ]
        var newIds = [ ]
        if (_multipleVehicles) {
            for (var i = 0; i < QGroundControl.multiVehicleManager.vehicles.count; i++) {
                var vehicle = QGroundControl.multiVehicleManager.vehicles.get(i)
                newModel.push(vehicle.name)
                newIds.push(vehicle.id)

                if (vehicle.id === _activeVehicle.id) {
                    newCurrentIndex = i
                }
            }
        }
        currentIndex = -1
        _vehicleModel = newModel
        _vehicleIds = newIds
        currentIndex = newCurrentIndex
    }

    onActivated: {
        var vehicle = QGroundControl.multiVehicleManager.getVehicleById(_vehicleIds[index])
        if (vehicle) {
            // Do not attempt to set the active vehicle to null if the index for any reason
            // was wrong (should not happen)
            QGroundControl.multiVehicleManager.activeVehicle = vehicle
        } else {
            // If there for some reason is a problem with the index
            // (should not happen), reset the model
            _updateVehicleModel()
        }
    }
}
