import QtQuick 2.12

import QGroundControl              1.0
import QGroundControl.Controls     1.0
import QGroundControl.ScreenTools  1.0
import QGroundControl.Palette      1.0

Item {
    id:     root
    width:  size
    height: size

    property real size:     ScreenTools.defaultFontPixelHeight * 5
    property var  vehicle:  null

    property real _windSpeed:     vehicle && vehicle.wind ? vehicle.wind.speed.rawValue : NaN
    property real _windDirection: vehicle && vehicle.wind ? vehicle.wind.direction.rawValue : NaN
    property real _windVariance:  vehicle && vehicle.wind ? vehicle.wind.variance.rawValue : NaN
    property bool _hasWind:       !isNaN(_windSpeed) && !isNaN(_windDirection)
    // Angular uncertainty (degrees): atan(variance / speed) * 2 for 2σ, clamped to [5, 120]
    property real _angleSigma2: {
        if (!_hasWind || isNaN(_windVariance) || _windVariance <= 0 || _windSpeed < 0.1)
            return 60
        var sigma = Math.atan2(_windVariance, _windSpeed) * 180 / Math.PI
        return Math.max(5, Math.min(sigma * 2, 120))
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    Rectangle {
        anchors.fill:   parent
        radius:         width / 2
        color:          qgcPal.window
        border.color:   qgcPal.text
        border.width:   1
    }

    // Sector + arrow drawn via Canvas
    Canvas {
        id:             windCanvas
        anchors.fill:   parent
        visible:        _hasWind

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            var cx = width / 2
            var cy = height / 2
            var r  = width / 2 - 2

            // Wind direction in canvas radians (0 = up = north, clockwise)
            // Canvas: 0rad = right, so north = -PI/2
            var dirRad = (_windDirection - 90) * Math.PI / 180
            var halfSpread = _angleSigma2 * Math.PI / 360

            // Draw sector (±2σ)
            ctx.beginPath()
            ctx.moveTo(cx, cy)
            ctx.arc(cx, cy, r * 0.92, dirRad - halfSpread, dirRad + halfSpread, false)
            ctx.closePath()
            ctx.fillStyle = Qt.rgba(0.3, 0.6, 1.0, 0.3)
            ctx.fill()

            // Draw mean direction line
            var lineLen = r * 0.92
            var ex = cx + lineLen * Math.cos(dirRad)
            var ey = cy + lineLen * Math.sin(dirRad)
            ctx.beginPath()
            ctx.moveTo(cx, cy)
            ctx.lineTo(ex, ey)
            ctx.lineWidth = 2
            ctx.strokeStyle = Qt.rgba(0.3, 0.6, 1.0, 1.0)
            ctx.stroke()

            // Draw arrowhead
            var headLen = r * 0.2
            var headAngle = 25 * Math.PI / 180
            var ax1 = ex - headLen * Math.cos(dirRad - headAngle)
            var ay1 = ey - headLen * Math.sin(dirRad - headAngle)
            var ax2 = ex - headLen * Math.cos(dirRad + headAngle)
            var ay2 = ey - headLen * Math.sin(dirRad + headAngle)
            ctx.beginPath()
            ctx.moveTo(ex, ey)
            ctx.lineTo(ax1, ay1)
            ctx.lineTo(ax2, ay2)
            ctx.closePath()
            ctx.fillStyle = Qt.rgba(0.3, 0.6, 1.0, 1.0)
            ctx.fill()
        }

        onVisibleChanged: if (visible) requestPaint()
    }

    // Repaint when values change
    Connections {
        target: root
        function on_WindDirectionChanged() { windCanvas.requestPaint() }
        function on_AngleSigma2Changed()   { windCanvas.requestPaint() }
        function on_HasWindChanged()       { windCanvas.requestPaint() }
    }

    // Speed range label
    Rectangle {
        anchors.centerIn:   parent
        width:              speedLabel.width + root.size * 0.12
        height:             speedLabel.height + root.size * 0.06
        radius:             height * 0.3
        color:              qgcPal.window
        opacity:            0.85
        visible:            _hasWind

        QGCLabel {
            id:                 speedLabel
            anchors.centerIn:   parent
            font.pointSize:     ScreenTools.defaultFontPointSize * (root.size / (ScreenTools.defaultFontPixelHeight * 5)) * 0.75
            color:              qgcPal.text
            text: {
                if (!_hasWind) return "---"
                var v = isNaN(_windVariance) || _windVariance <= 0 ? 0 : _windVariance * 2
                var lo = Math.max(0, _windSpeed - v).toFixed(1)
                var hi = (_windSpeed + v).toFixed(1)
                return lo + " - " + hi
            }
        }
    }
}
