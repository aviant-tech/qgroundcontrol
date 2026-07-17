/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import QGroundControl
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.FactSystem
import QGroundControl.Palette
import QGroundControl.Vehicle

Rectangle {
    enum WinchCommands {
        // MAVLink definitions not yet implemented in QGC.
        // Using the actions from https://mavlink.io/en/messages/common.html#WINCH_ACTIONS
        WINCH_RELAXED = 0,
        WINCH_DELIVER = 4,
        WINCH_RETRACT = 6
    }

    id:                         winchControlRect
    color:                      qgcPal.window
    radius:                     _toolsMargin / 2
    height:                     winchControlLayout.height + (_toolsMargin * 2)

    property var  _vehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property real _toolsMargin: ScreenTools.defaultFontPixelWidth * 0.75
    property real _child_width: width - (_toolsMargin * 2)

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    function uncheckInactiveButtons() {
        winchEmergencyBtn.checked = false
        winchDeliverBtn.checked = false
        winchRetractBtn.checked = false

        switch(slider._currentWinchCommand) {
            case null:
                return
            case WinchControl.WinchCommands.WINCH_RELAXED:
                winchEmergencyBtn.checked = true
                break
            case WinchControl.WinchCommands.WINCH_DELIVER:
                winchDeliverBtn.checked = true
                break
            case WinchControl.WinchCommands.WINCH_RETRACT:
                winchRetractBtn.checked = true
                break
        }
    }

    function getValue(name) {
        // Convenience function for pretty print numeric winch status with name and unit
        if (!_vehicle) {
            return "Not connected"
        }
        let fact = null
        switch(name) {
            case "lineLength":
                fact = _vehicle.winchStatus.lineLength
                break
            case "tension":
                fact = _vehicle.winchStatus.tension
                break
            case "speed":
                fact = _vehicle.winchStatus.speed
                break
            default:
                break
        }
        if (fact) {
            return fact.shortDescription + ": " + fact.valueString + " " + fact.units
        } else {
            return name + " unknown"
        }
    }

    function getStatus() {
        // Returns the current winch status as a short text, and a colour to indicate severity,
        // Green:  Winch is homed and OK
        // Yellow: Some normal operation is in progress
        // Orange: Warning, winch state requires pilot attention
        // Red:    Critical error
        if (!_vehicle) {
            return {"color": "red", "text": "Vehicle not connected"}
        } else if (_vehicle.winchStatus.stale.rawValue) {
            return {"color": "red", "text": "No data from winch"}
        } else if (_vehicle.winchStatus.clutchEngaged.rawValue) {
            return {"color": "orange", "text": "Emergency mode"}
        } else if (!_vehicle.winchStatus.healthy.rawValue) {
            return {"color": "red", "text": "Winch error"}
        } else if (_vehicle.winchStatus.fullyRetracted.rawValue) {
            if (_vehicle.winchStatus.locking.rawValue) {
                return {"color": "yellow", "text": "Locking"}
            } else if (_vehicle.winchStatus.locked.rawValue) {
                return {"color": "green", "text": "Home"}
            } else {
                return {"color": "orange", "text": "Home with locking error"}
            }
        } else if (_vehicle.winchStatus.dropping.rawValue || _vehicle.winchStatus.arresting.rawValue) {
            return {"color": "yellow", "text": "Dropping"}
        } else if (_vehicle.winchStatus.retracting.rawValue) {
            if (_vehicle.winchStatus.redeliver.rawValue) {
                return {"color": "orange", "text": "Retracting (high load)"}
            } else {
                return {"color": "yellow", "text": "Retracting"}
            }
        } else if (_vehicle.winchStatus.moving.rawValue) {
            return {"color": "yellow", "text": "Moving"}
        } else {
            return {"color": "yellow", "text": "Holding"}
        }
    }

    ColumnLayout {
        id:              winchControlLayout
        anchors.margins: _toolsMargin
        anchors.bottom:  parent.bottom
        anchors.left:    parent.left
        spacing:         _toolsMargin * 2

        QGCButton {
            id:                    winchEmergencyBtn
            backRadius:            10
            showBorder:            true
            Layout.preferredWidth: parent.parent._child_width
            text:                  "RELEASE"
            font.bold:             true
            background:            Rectangle {
                color:             "red"
                radius:            10
                border.color:      "white"
                border.width:      1
            }
            hoverEnabled:          false
            checkable:             true
            Image {
                id:                alert
                source:            "/qmlimages/Yield.svg"
                height:            parent.height
                width:             height
                x:                 10
            }
            onClicked:             winchRelax()

            function winchRelax() {
                if (checked) {
                    slider.enableWinchSlider(WinchControl.WinchCommands.WINCH_RELAXED, qsTr("Release"))
                    uncheckInactiveButtons()
                } else {
                    slider.disableWinchSlider()
                }
            }
        }

        Rectangle {
            property var _status:         getStatus()
            id:                           winchStatus
            height:                       winchStatusChild.height + 2 * _toolsMargin
            Layout.preferredWidth:        parent.parent._child_width
            radius:                       _toolsMargin
            color:                        _status.color
            QGCLabel {
                id:                       winchStatusChild
                text:                     parent._status.text
                color:                    "black"
                font.bold:                true
                anchors.verticalCenter:   parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        QGCLabel {
            text: getValue("lineLength")
        }

        QGCLabel {
            text: getValue("tension")
        }

        // Show why drop is blocked, currently not used to reduce noise in interface
        // QGCLabel {
        //     text: _vehicle ? _vehicle.winchStatus.dropBlocker.enumOrValueString : ""
        // }

        QGCButton {
            id:                         winchDeliverBtn
            backRadius:                 10
            showBorder:                 true
            Layout.preferredWidth:      parent.parent._child_width
            text:                       "DELIVER"
            hoverEnabled:               false
            checkable:                  true
            enabled:                    _vehicle ? _vehicle.winchStatus.dropAllowed.rawValue : false
            Image {
                id:                     downArrow
                source:                 "/qmlimages/ArrowDirection.svg"
                height:                 parent.height
                width:                  height
                x:                      10
                transform: Rotation {
                    origin.x:           downArrow.width/2
                    origin.y:           downArrow.height/2
                    angle:              180
                }
            }
            onClicked:                  winchDeliver()

            function winchDeliver() {
                if (checked) {
                    slider.enableWinchSlider(WinchControl.WinchCommands.WINCH_DELIVER, qsTr("Drop"))
                    uncheckInactiveButtons()
                } else {
                    slider.disableWinchSlider()
                }
            }
        }

        QGCButton {
            id:                    winchRetractBtn
            backRadius:            10
            showBorder:            true
            Layout.preferredWidth: parent.parent._child_width
            text:                  "RETRACT"
            hoverEnabled:          false
            checkable:             true
            Image {
                id:                upArrow
                source:            "/qmlimages/ArrowDirection.svg"
                height:            parent.height
                width:             height
                x:                 10
            }
            onClicked:             winchRetract()

            function winchRetract() {
                if (checked) {
                    slider.enableWinchSlider(WinchControl.WinchCommands.WINCH_RETRACT, qsTr("Retract"))
                    uncheckInactiveButtons()
                } else {
                    slider.disableWinchSlider()
                }
            }
        }

        Item {
            height:                                slider.implicitHeight
            SliderSwitch {
                id:                                slider
                visible:                           false
                width:                             parent.parent.parent._child_width
                confirmText:                       qsTr("Not visible")

                property var _currentWinchCommand: null
                onAccept:                          sendWinchCommand()

                function enableWinchSlider(command, message) {
                    visible = true
                    confirmText = message
                    _currentWinchCommand = command
                }

                function disableWinchSlider() {
                    visible = false
                    confirmText = qsTr("Not visible")
                    _currentWinchCommand = null
                }

                function sendWinchCommand() {
                    if (_vehicle === null) {
                        console.log("Cannot send winch command, vehicle not connected.")
                    } else if (_currentWinchCommand === null) {
                        console.log("Cannot send winch command, no action selected.")
                    } else {
                        // MAV_COMP_ID_USER18(42) was the winch component in the 1.13-era
                        // firmware; MAV_COMP_ID_WINCH(169) is the dedicated winch component
                        // used from 1.15 onward (the Notus companion listens on 169). We send
                        // to both for cross-firmware compatibility.
                        //
                        // Error surfacing: comp 169 is the current, authoritative winch
                        // component, so we send it with showError=true — if the real winch
                        // does not acknowledge the command, the pilot gets a "Vehicle did not
                        // respond to command" warning instead of the failure being swallowed.
                        // The legacy comp 42 send is expected to go unacknowledged on modern
                        // firmware, so it stays showError=false to avoid a spurious popup on
                        // every winch action.
                        //
                        // The QML MAVLink enum doesn't include MAV_CMD_DO_WINCH(42600).
                        // Setting it explicitly. See src/MAVLink/QGCMAVLink.h for details.
                        // Length (param3) is set to 1m here, this is not used but it is
                        // IMPORTANT that param3 is 1 since it is used by winch code to
                        // distinguish between manual and mission commands.
                        _vehicle.sendCommand(42,  42600, 0 /*showError:false*/, 1, _currentWinchCommand, 1 /*MANUAL*/, 1)
                        _vehicle.sendCommand(169, 42600, 1 /*showError:true*/,  1, _currentWinchCommand, 1 /*MANUAL*/, 1)
                        disableWinchSlider()
                        uncheckInactiveButtons()
                    }
                }
            }
        }
    }
}
