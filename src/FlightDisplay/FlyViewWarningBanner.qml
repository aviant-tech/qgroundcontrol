/****************************************************************************
 *
 * (c) 2026 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick 2.12

import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0

/// Warning banner at the top of the fly view, centered in its parent
Rectangle {
    property alias text:        label.text
    property alias textColor:   label.color
    /// Shows a close button, which emits `dismissed`
    property bool  dismissable: false

    signal dismissed

    anchors.horizontalCenter:   parent.horizontalCenter
    width:                      row.width + ScreenTools.defaultFontPixelWidth * 2
    height:                     row.height + ScreenTools.defaultFontPixelHeight
    color:                      "red"
    radius:                     ScreenTools.defaultFontPixelWidth / 2

    Row {
        id:                 row
        anchors.centerIn:   parent
        spacing:            ScreenTools.defaultFontPixelWidth

        QGCLabel {
            id:                     label
            anchors.verticalCenter: parent.verticalCenter
            color:                  "white"
            font.bold:              true
            font.pointSize:         ScreenTools.largeFontPointSize
        }

        QGCColoredImage {
            anchors.verticalCenter: parent.verticalCenter
            width:                  ScreenTools.isMobile ? ScreenTools.defaultFontPixelHeight * 1.5 : ScreenTools.defaultFontPixelHeight
            height:                 width
            sourceSize.height:      width
            source:                 "/res/XDelete.svg"
            fillMode:               Image.PreserveAspectFit
            color:                  label.color
            visible:                dismissable

            MouseArea {
                anchors.fill:       parent
                anchors.margins:    -ScreenTools.defaultFontPixelHeight
                onClicked:          dismissed()
            }
        }
    }
}
