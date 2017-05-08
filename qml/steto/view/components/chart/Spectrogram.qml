import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    anchors.fill: parent

    function requestPaint() {
        canvas.requestPaint()
    }

    Canvas {
        id: canvas

        anchors {
            fill: parent
        }

        contextType: "2d"

        property real w: model.length > 1 ? canvas.width / Math.max(model.length - 1, minViewport) : 0

        onPaint: {
            var ctx = canvas.context

            ctx.clearRect(0, 0, canvas.width, canvas.height)

            if(model.length > 0 && range > 0) {
                var gradient = ctx.createLinearGradient(0, 0, 0, canvas.height);
                gradient.addColorStop(0.0, "#d0ffffff");
                gradient.addColorStop(1.0, "#00ffffff");

                ctx.lineWidth = 2
                ctx.lineJoin = "round"
                ctx.strokeStyle = "#fff"
                ctx.fillStyle = gradient

                ctx.beginPath()

                ctx.moveTo(xvalue(0), yvalue(0))

                for(var i = 1; i < model.length; ++i) {
                    ctx.lineTo(xvalue(i), yvalue(i))
                }

                ctx.stroke()

                ctx.lineTo(canvas.width, canvas.height)
                ctx.lineTo(0, canvas.height)

                ctx.closePath()

                ctx.fill()
            }
        }

        function xvalue(i) {
            return i * w
        }

        function yvalue(i) {
            return canvas.height - 0.8 * canvas.height * model[i] / range
        }
    }
}
