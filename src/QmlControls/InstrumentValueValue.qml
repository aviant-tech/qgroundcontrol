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
// value text, so limit states are more visible on the editable telemetry bar.
Item {
    property var    instrumentValueData:            null
    property bool   settingsUnlocked:               false
    property alias  contentWidth:                   label.contentWidth

    property var    _rgFontSizes:                   [ ScreenTools.defaultFontPointSize, ScreenTools.smallFontPointSize, ScreenTools.mediumFontPointSize, ScreenTools.largeFontPointSize ]
    property var    _rgFontSizeRatios:              [ 1, ScreenTools.smallFontPointRatio, ScreenTools.mediumFontPointRatio, ScreenTools.largeFontPointRatio ]
    property real   _doubleDescent:                 ScreenTools.defaultFontDescent * 2
    property real   _tightDefaultFontHeight:        ScreenTools.defaultFontPixelHeight - _doubleDescent
    property var    _rgFontSizeTightHeights:        [ _tightDefaultFontHeight * _rgFontSizeRatios[0] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[1] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[2] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[3] + 2 ]
    property real   _tightHeight:                   _rgFontSizeTightHeights[instrumentValueData.factValueGrid.fontSize]
    property real   _fontSize:                      _rgFontSizes[instrumentValueData.factValueGrid.fontSize]

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    Layout.fillHeight:      true
    Layout.fillWidth:       true
    Layout.alignment:       Qt.AlignVCenter
    Layout.preferredHeight: _tightHeight + ScreenTools.defaultFontPixelHeight

    Rectangle {
        anchors.fill:       parent
        anchors.leftMargin: -ScreenTools.defaultFontPixelWidth
        color:              instrumentValueData.isValidColor(instrumentValueData.currentColor) ? instrumentValueData.currentColor : "transparent"
        opacity:            instrumentValueData.currentOpacity

        QGCLabel {
            id:                     label
            anchors.fill:           parent
            horizontalAlignment:    Text.AlignHCenter
            verticalAlignment:      Text.AlignVCenter
            font.pointSize:         _fontSize
            color:                  qgcPal.text
            text:                   valueText()

            function valueText() {
                if (instrumentValueData.fact) {
                    return instrumentValueData.fact.enumOrValueString + (instrumentValueData.showUnits ? " " + instrumentValueData.fact.units : "")
                } else {
                    return qsTr("--.--")
                }
            }
        }
    }
}
