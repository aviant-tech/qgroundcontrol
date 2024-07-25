
import QtQuick          2.12
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0


Rectangle {
    id:      vehicleWarningSideBarRect
    color:   qgcPal.window
    radius:  _toolsMargin / 2
    opacity: 0.8

    // Overridden by parent
    property int maxWidth:              400
    property int minWidth:              200
    property int availableHeight:       600

    property ListModel errorListModel:  ListModel {}
    property var _activeVehicle:        QGroundControl.multiVehicleManager.activeVehicle
    property int maxHeight:             parent.height * 0.5
    property int maxWidthBasedOnWindow: parent.width * 0.25
    
    height:  Math.min(Math.min(vehicleWarningSideBarLayout.implicitHeight + ScreenTools.defaultFontPixelWidth, maxHeight), availableHeight)
    width:   Math.min(Math.max(maxWidthBasedOnWindow, minWidth), maxWidth)
    visible: _activeVehicle && errorListModel.count > 0

    MouseArea {
        anchors.fill: parent
        onWheel:      {}
    }

    ColumnLayout {
        id:              vehicleWarningSideBarLayout
        anchors.fill:    parent
        anchors.margins: ScreenTools.defaultFontPixelWidth / 2

        Item {
            Layout.fillWidth: true
            height: trashDelete.height

            QGCColoredImage {
                id:            trashDelete
                height:        ScreenTools.isMobile ? ScreenTools.defaultFontPixelHeight * 1.5 : ScreenTools.defaultFontPixelHeight
                width:         height
                source:        "/res/TrashDelete.svg"
                color:         qgcPal.text
                visible:       errorListModel.count > 0
                anchors.right: parent.right
                MouseArea {
                    anchors.fill: parent
                    onClicked:    errorListModel.clear()
                }
            }
        }

        ListView {
            id:                      errorListView
            model:                   errorListModel
            clip:                    true
            spacing:                 ScreenTools.defaultFontPixelWidth / 2
            verticalLayoutDirection: ListView.BottomToTop
            Layout.fillHeight:       true
            Layout.fillWidth:        true
            Layout.preferredHeight:  contentHeight

            delegate: Rectangle {
                id: warningItem
                width:  errorListView.width
                color:  qgcPal.alertBackground
                height: rowLayout.implicitHeight + ScreenTools.defaultFontPixelWidth
                radius: 5

                property bool isNew: true

                 SequentialAnimation {
                    id: newWarningAnimation
                    running: warningItem.isNew
                    loops: 1
                    ParallelAnimation {
                        ColorAnimation {
                            target: warningItem
                            property: "color"
                            from: qgcPal.alertBackground
                            to: "yellow"
                            duration: 200
                        }
                        NumberAnimation {
                            target: warningItem
                            property: "scale"
                            from: 1.0
                            to: 1.025
                            duration: 200
                            easing.type: Easing.OutQuad
                        }
                    }
                    ParallelAnimation {
                        ColorAnimation {
                            target: warningItem
                            property: "color"
                            from: "yellow"
                            to: qgcPal.alertBackground
                            duration: 200
                        }
                        NumberAnimation {
                            target: warningItem
                            property: "scale"
                            from: 1.025
                            to: 1.0
                            duration: 200
                            easing.type: Easing.InQuad
                        }
                    }
                    onStopped: warningItem.isNew = false
                }

                RowLayout {
                    id:              rowLayout
                    anchors.fill:    parent
                    anchors.margins: ScreenTools.defaultFontPixelWidth / 2

                    QGCLabel {
                        text:                  errorMessage
                        color:                 qgcPal.alertText
                        font.pointSize:        ScreenTools.mediumFontPointSize
                        wrapMode:              Text.WordWrap
                        Layout.fillWidth:      true
                    }

                    QGCColoredImage {
                        id:                 criticalVehicleMessageClose
                        width:              ScreenTools.isMobile ? ScreenTools.defaultFontPixelHeight * 1.5 : ScreenTools.defaultFontPixelHeight
                        height:             width
                        sourceSize.height:  width
                        source:             "/res/XDelete.svg"
                        fillMode:           Image.PreserveAspectFit
                        color:              qgcPal.alertText
                        Layout.alignment:   Qt.AlignVCenter

                        MouseArea {
                            anchors.fill:    parent
                            anchors.margins: -ScreenTools.defaultFontPixelHeight
                            onClicked:       errorListModel.remove(index)
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: _activeVehicle
        // This currently only catches warnings from the active vehicle. If we want to support multiple vehicles, we need to change this.
        onNewCriticalVehicleMessage :{
            // If we want to support multiple vehicles, we need to display the drone name in the message.
            errorListModel.insert(0, {"errorMessage": message, "isNew": true})
        }
    }
}