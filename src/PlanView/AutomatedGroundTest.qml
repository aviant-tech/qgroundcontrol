import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts  1.2
import QtPositioning    5.2

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

QGCFlickable {
    id:             root
    contentHeight:  automatedGroundTestRect.height
    clip:           true

    property var    automatedGroundTest
    property var    flightMap


    readonly property real  _editFieldWidth:    Math.min(width - _margin * 2, ScreenTools.defaultFontPixelWidth * 15)
    readonly property real  _margin:            ScreenTools.defaultFontPixelWidth / 2
    readonly property real  _radius:            ScreenTools.defaultFontPixelWidth / 2

    Rectangle {
        id:     automatedGroundTestRect
        anchors.left:   parent.left
        anchors.right:  parent.right
        height: automatedGroundTestItems.y + automatedGroundTestItems.height + (_margin * 2)
        radius: _radius
        color:  qgcPal.missionItemEditor

        QGCLabel {
            id:                 automatedGroundTestLabel
            anchors.margins:    _margin
            anchors.left:       parent.left
            anchors.top:        parent.top
            text:               qsTr("Automated Ground Test")
            anchors.leftMargin: ScreenTools.defaultFontPixelWidth
        }

        Rectangle {
            id:                 automatedGroundTestItems
            anchors.margins:    _margin
            anchors.left:       parent.left
            anchors.right:      parent.right
            anchors.top:        automatedGroundTestLabel.bottom
            height:             fenceColumn.y + fenceColumn.height + (_margin * 2)
            color:              qgcPal.windowShadeDark
            radius:             _radius

            Column {
                id:                 fenceColumn
                anchors.margins:    _margin
                anchors.top:        parent.top
                anchors.left:       parent.left
                anchors.right:      parent.right
                spacing:            _margin

                QGCLabel {
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    wrapMode:           Text.WordWrap
                    font.pointSize:     ScreenTools.smallFontPointSize
                    text:               qsTr(automatedGroundTest.statusText)
                }

                QGCButton {
                        property string     buttonText
                        property string     buttonColor
                        property bool       buttonVisible: false
                        property bool       alertVisible: false

                        id:                         button
                        visible:                    button.visible
                        backRadius:                 10
                        showBorder:                 true
                        width:                      parent.width
                        text:                       buttonText
                        background:                 Rectangle {

                            color:                  button.buttonColor
                            radius:                 10
                            border.color:           "white"
                            border.width:           1
                        }
                        checkable:                  true
                        Image {
                            id:                     alert
                            visible:                button.alertVisible
                            source:                 "/qmlimages/Yield.svg"
                            height:                 parent.height
                            width:                  height
                            x:                      10
                        }

                        onClicked: {
                            automatedGroundTest.buttonClicked()
                        }

                        Connections {
                            target: _automatedGroundTest
                            onCurrentStateChanged: {
                                button.updateButtonState()
                            }
                        }

                        function updateButtonState()
                        {
                            switch (automatedGroundTest.currentState) {

                            case AutomatedGroundTest.Idle:
                                button.visible = false
                                break

                            case AutomatedGroundTest.FileLoaded:
                            case AutomatedGroundTest.Stop:
                                button.buttonColor = "green"
                                button.buttonText = "START"
                                button.visible = true
                                button.alertVisible = false
                                break

                            case AutomatedGroundTest.Running:
                                button.buttonColor = "red"
                                button.buttonText = "STOP"
                                button.visible = true
                                button.alertVisible = true
                                break
                            }
                        }
                    } // QGCButton
            }

        } // Rectangle
    } // Rectangle
}
