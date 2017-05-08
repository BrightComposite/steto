import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    anchors.fill: parent

    function requestPaint() {
        canvas.requestPaint()
    }

    Rectangle {
        anchors.centerIn: parent
        width: parent.width
        height: 1
        color: "#10ffffff"
    }

    Canvas {
        id: canvas

        anchors {
            fill: parent
            margins: -5
        }

        contextType: "2d"

        property real w: model.length > 1 ? canvas.width / Math.max(model.length - 1, minViewport) : 0

        onPaint: {
            var ctx = canvas.context

            ctx.clearRect(0, 0, canvas.width, canvas.height)

            if(model.length > 0 && range > 0) {
                ctx.lineJoin = "round"
                ctx.lineWidth = 1
                ctx.strokeStyle = "#7df"
                ctx.beginPath()

                ctx.moveTo(xvalue(0), yvalue(0))

                for(var i = 1; i < model.length; ++i) {
                    ctx.lineTo(xvalue(i), yvalue(i))
                }

                ctx.stroke()
            }
        }

        function xvalue(i) {
            return i * w
        }

        function yvalue(i) {
            return 0.5 * canvas.height - 0.8 * canvas.height * model[i] / range
        }

        visible: false
    }

    Glow {
        anchors.fill: canvas
        source: canvas
        samples: 15
        spread: 0.25
        color: "#a0aaeeff"
    }
}
