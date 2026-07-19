/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.Palette

// Aviant (A67): the range/limit colour is drawn as the cell background instead of colouring the
// icon/label, so limit states are more visible on the editable telemetry bar.
Item {
    property var    instrumentValueData:        null

    property var    _rgFontSizes:               [ ScreenTools.defaultFontPointSize, ScreenTools.smallFontPointSize, ScreenTools.mediumFontPointSize, ScreenTools.largeFontPointSize ]
    property var    _rgFontSizeRatios:          [ 1, ScreenTools.smallFontPointRatio, ScreenTools.mediumFontPointRatio, ScreenTools.largeFontPointRatio ]
    property real   _doubleDescent:             ScreenTools.defaultFontDescent * 2
    property real   _tightDefaultFontHeight:    ScreenTools.defaultFontPixelHeight - _doubleDescent
    property var    _rgFontSizeTightHeights:    [ _tightDefaultFontHeight * _rgFontSizeRatios[0] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[1] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[2] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[3] + 2 ]
    property real   _tightHeight:               _rgFontSizeTightHeights[instrumentValueData.factValueGrid.fontSize]
    property bool   _iconVisible:               instrumentValueData.rangeType === InstrumentValueData.IconSelectRange || instrumentValueData.icon

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    Layout.fillHeight:      true
    Layout.fillWidth:       true
    Layout.alignment:       Qt.AlignVCenter
    Layout.preferredWidth:  label.contentWidth + (_iconVisible ? valueIcon.width : 0) + (ScreenTools.defaultFontPixelWidth * 2)
    Layout.minimumWidth:    Layout.preferredWidth

    Rectangle {
        anchors.fill:   parent
        color:          instrumentValueData.isValidColor(instrumentValueData.currentColor) ? instrumentValueData.currentColor : "transparent"
        opacity:        instrumentValueData.currentOpacity

        QGCColoredImage {
            id:                         valueIcon
            anchors.centerIn:           parent
            height:                     parent.height * 0.75
            width:                      height
            sourceSize.height:          height
            fillMode:                   Image.PreserveAspectFit
            mipmap:                     true
            smooth:                     true
            color:                      qgcPal.text
            visible:                    _iconVisible

            readonly property string iconPrefix: "/InstrumentValueIcons/"

            function updateIcon() {
                if (instrumentValueData.rangeType === InstrumentValueData.IconSelectRange) {
                    valueIcon.source = instrumentValueData.currentIcon != "" ? iconPrefix + instrumentValueData.currentIcon : "";
                } else if (instrumentValueData.icon) {
                    valueIcon.source = instrumentValueData.icon != "" ? iconPrefix + instrumentValueData.icon : "";
                } else {
                    valueIcon.source = ""
                }
            }

            Connections {
                target:                 instrumentValueData
                function onRangeTypeChanged() { valueIcon.updateIcon() }
                function onCurrentIconChanged() { valueIcon.updateIcon() }
                function onIconChanged() { valueIcon.updateIcon() }
            }
            Component.onCompleted:      updateIcon();

            Rectangle {
                anchors.fill:   valueIcon
                color:          qgcPal.text
                visible:        valueIcon.status === Image.Error
            }
        }

        QGCLabel {
            id:                     label
            anchors.fill:           parent
            horizontalAlignment:    Text.AlignHCenter
            verticalAlignment:      Text.AlignVCenter
            font.pointSize:         ScreenTools.smallFontPointSize
            text:                   instrumentValueData.text
            visible:                !_iconVisible
            color:                  qgcPal.text
        }
    }
}
