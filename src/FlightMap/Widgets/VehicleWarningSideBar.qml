
import QtQuick          2.12
import QtQuick.Layouts  1.12
import QtQuick.Window   2.2
import QtQuick.Controls 2.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette       1.0
import QGroundControl.Vehicle       1.0

import MAVLink                      1.0

Rectangle {
    id: vehicleWarningSideBarRect
    color: qgcPal.window
    radius: _toolsMargin / 2

    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property ListModel errorListModel: ListModel {}

    // Center the rectangle in the parent
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.margins:    -ScreenTools.defaultFontPixelHeight

    // Set explicit width and height based on the content or specific dimensions
    width: vehicleWarningSideBarLayout.implicitWidth + (_toolsMargin * 2)
    height: 500
    // visible: _activeVehicle && errorListModel.count > 0

    ColumnLayout {
        id: vehicleWarningSideBarLayout
        spacing: _toolsMargin * 2
        anchors.fill: parent
        anchors.margins: 10

       QGCLabel {
            text: errorListModel.count > 0 && errorListModel.get(0).errorMessage !== "No formatted messages" 
                ? "Warnings" 
                : "No Warnings"
            font.pointSize: ScreenTools.mediumFontPointSize
        }

        ListView {
            id: errorListView
            model: errorListModel
            orientation: ListView.Vertical
            clip: true
            spacing: 10
            Layout.fillHeight: true
            Layout.fillWidth: true

            delegate: Rectangle {
                width: vehicleWarningSideBarRect.width - 20
                height: 50
                color: qgcPal.colorOrange
                radius: 5

                Row {
                    anchors.fill: parent

                    QGCLabel {
                        text: errorMessage
                        color: "black"
                    }

                    Button {
                        text: "Dismiss"
                        onClicked: errorListModel.remove(index)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        updateErrorList()
    }

    function updateErrorList() {
        console.log("start")
        console.log(_activeVehicle)
        if (_activeVehicle) {
            console.log(_activeVehicle.formattedMessages)
            console.log(_activeVehicle.latestError)
            console.log(_activeVehicle.messagesReceived)
            console.log(_activeVehicle.newMessageCount)
        }

        errorListModel.clear()
        if (_activeVehicle && _activeVehicle.formattedMessages) {
            var messages = _activeVehicle.formattedMessages.split('\n');
            for (var i = 0; i < messages.length; i++) {
                if (messages[i].trim() !== '') {
                    errorListModel.append({"errorMessage": messages[i]});
                }
            }
        }
        errorListModel.append({"errorMessage": "test"});

    }

    Connections {
        target: _activeVehicle
        onNewFormattedMessage :{
            console.log("append!!")
            console.log(formattedMessage)
            errorListModel.append({"errorMessage": formattedMessage})

        }
    }

}