/****************************************************************************
 *
 * (c) 2009-2023 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/
import QtQuick                  2.12
import QtQuick.Controls         2.4
import QtQuick.Dialogs          1.3
import QtQuick.Layouts          1.12

import QtLocation               5.3
import QtPositioning            5.3
import QtQuick.Window           2.2
import QtQml.Models             2.1

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Airspace      1.0
import QGroundControl.Airmap        1.0
import QGroundControl.Controllers   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0

Rectangle {

    property int _selectedBranchItem : _missionController.branchItem(branchesComboBox.currentIndex)
    property bool _visible : false

    Connections {
        target: guidedActionsController
        function onSetBranchesVisibility(isVisable) {
            _root._visible = isVisable
        }
    }

    id:         _root
    visible:    _visible
    anchors.horizontalCenter:   parent.horizontalCenter
    anchors.verticalCenter:     parent.verticalCenter
    color:                      qgcPal.windowShadeDark
    radius:                     _toolsMargin
    height:                     branchSelectorLayout.height * 1.3
    width:                      branchSelectorLayout.width * 1.4

    ColumnLayout {
        id:              branchSelectorLayout
        anchors.margins: _toolsMargin * 2
        anchors.bottom:  parent.bottom
        anchors.left:    parent.left
        spacing:         _toolsMargin * 2

        QGCLabel {
            id:                 branchSelectorLabel
            text:               qsTr("Select the branch item to jump:")
        }

        QGCComboBox {

            id : branchesComboBox

            property var _branchItemNames : _missionController.branchItemNames

            Connections {
                target: _missionController
                onBranchItemNamesChanged: {
                    branchesComboBox._branchItemNames = _missionController.branchItemNames
                }
            }

            Layout.minimumWidth: parent.width
            model: _branchItemNames

            onActivated: {
                _root._selectedBranchItem = _missionController.branchItem(branchesComboBox.currentIndex)
            }
        }
    }
}


