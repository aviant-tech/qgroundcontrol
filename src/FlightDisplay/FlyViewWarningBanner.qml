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

    anchors.horizontalCenter:   parent.horizontalCenter
    width:                      label.width + ScreenTools.defaultFontPixelWidth * 2
    height:                     label.height + ScreenTools.defaultFontPixelHeight
    color:                      "red"
    radius:                     ScreenTools.defaultFontPixelWidth / 2

    QGCLabel {
        id:                 label
        anchors.centerIn:   parent
        color:              "white"
        font.bold:          true
        font.pointSize:     ScreenTools.largeFontPointSize
    }
}
