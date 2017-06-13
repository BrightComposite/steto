import QtQuick 2.0
import QtGraphicalEffects 1.0

ChartDelegate {
    property var areas: []

    property int offset: 0
    property real step: 1
    property real selected: -1

    function update() {
        selected = -1
        canvas.requestPaint()
    }

    onAreasChanged: {
        canvas.requestPaint()
    }

    Canvas {
        id: canvas

        anchors {
            fill: parent
        }

        contextType: "2d"

        property real w: chart.model.length > 1 ? canvas.width / Math.max(chart.model.length - 1, chart.minViewport) : 0

        onPaint: {
            var ctx = canvas.context
            ctx.clearRect(0, 0, canvas.width, canvas.height)

            ctx.reset()

            if(chart.model.length > 0 && chart.range > 0) {
                ctx.lineWidth = Math.max(1, w - 1)

                for(var i = 0; i < areas.length; ++i) {
                    var area = areas[i],
                        start = Math.round((area.start - offset) / step),
                        end = Math.round((area.end - offset) / step)

                    if(start >= 0 && start <= chart.model.length ||
                       end   >= 0 && end   <= chart.model.length ||
                       start <= 0 && end   >= chart.model.length) {
                        var c = area.color
                        var g = ctx.createLinearGradient(0, 0, 0, canvas.height)
                        g.addColorStop(0.0, Qt.rgba(c.r, c.g, c.b, c.a * 0.7))
                        g.addColorStop(1.0, Qt.rgba(c.r, c.g, c.b, c.a * 0.1))

                        ctx.fillStyle = g
                        ctx.fillRect(start * w, 0, (end - start) * w, canvas.height)

                        ctx.strokeStyle = Qt.rgba(c.r, c.g, c.b, c.a * 0.8)

                        ctx.beginPath()

                        ctx.moveTo(start * w, 0)
                        ctx.lineTo(start * w, canvas.height)
                        ctx.moveTo(end * w, 0)
                        ctx.lineTo(end * w, canvas.height)

                        ctx.stroke()
                    }
                }

                ctx.lineWidth = Math.max(1, w - 1)
                var gradient = ctx.createLinearGradient(0, 0, 0, canvas.height)

                gradient.addColorStop(0.0, "#ffffffff")
                gradient.addColorStop(1.0, "#30ffffff")

                ctx.fillStyle = gradient
                ctx.strokeStyle = gradient

                ctx.beginPath()

                for(var i = 0; i < chart.model.length; ++i) {
                    ctx.moveTo(xvalue(i), canvas.height)
                    ctx.lineTo(xvalue(i), yvalue(i))
                }

                ctx.stroke()

                if(selected >= 0) {
                    var g = ctx.createLinearGradient(0, 0, 0, canvas.height)
                    g.addColorStop(0.0, "#ffffffff");
                    g.addColorStop(1.0, "#a0ffffff");
                    ctx.strokeStyle = g

                    ctx.beginPath()
                    ctx.moveTo(xvalue(selected), canvas.height)
                    ctx.lineTo(xvalue(selected), yvalue(selected))
                    ctx.stroke()
                }
            }
        }

        function xvalue(i) {
            return i * w
        }

        function yvalue(i) {
            return canvas.height - 0.8 * canvas.height * chart.model[i] / chart.range
        }
    }

    Text {
        id: herzes

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 8
        }

        color: "#fff"
        text: selected >= 0 ? (step < 1 ? "~" : "") + Math.floor(100 * (selected * step + offset)) / 100 + "Hz" : ""

        visible: false
    }

    DropShadow {
        anchors.fill: herzes
        source: herzes
        samples: 16
        spread: 0.5
    }

    MouseArea {
        anchors.fill: parent

        preventStealing: true

        onClicked: {
            selected = Math.round(mouse.x / canvas.w)
            canvas.requestPaint()
        }
    }
}
