import QtQuick 2.0

import QtGraphicalEffects 1.0

Item {
    id: item

    property int lineWidth: 12
    property real progress: 0.0
    property string description: ""

    property color back: "#44ffffff"
    property color front: "#fff"
    property color textColor: "#fff"

    property alias progressLabel: progressLabel
    property alias descriptionLabel: descriptionLabel

    property int intProgress: progress * 10000

    onFrontChanged: {
        canvas.requestPaint()
    }

    onBackChanged: {
        canvas.requestPaint()
    }

    onIntProgressChanged: {
        canvas.requestPaint()
    }

    Behavior on intProgress {
        NumberAnimation {
            duration: 200
        }
    }

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d");
            ctx.save();
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            var cx = width / 2;
            var cy = height / 2;
            var r  = (width - item.lineWidth) / 2;
            var start = -Math.PI * 0.5;
            var middle = start + Math.PI * 2 * (intProgress / 10000);
            var end = start + Math.PI * 2;

            ctx.lineWidth = item.lineWidth;

            ctx.beginPath();
            ctx.strokeStyle = front
            ctx.arc(cx, cy, r, start, middle, false);
            ctx.stroke();

            ctx.beginPath();
            ctx.strokeStyle = back
            ctx.arc(cx, cy, r, middle, end, false);
            ctx.stroke();

            ctx.restore();
        }
    }

    Text {
        id: progressLabel
        text: "%1%".arg((intProgress / 100).toFixed())
        color: textColor

        horizontalAlignment: Qt.AlignHCenter

        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: description == "" ? parent.verticalCenter : undefined
            bottom: description != "" ? parent.verticalCenter : undefined
            bottomMargin: 2
        }

        font {
            pointSize: item.width / 7
            bold: true
        }
    }

    Text {
        id: descriptionLabel
        text: item.description
        color: "#fff"

        horizontalAlignment: Qt.AlignHCenter
        wrapMode: Text.WordWrap

        anchors {
            top: parent.verticalCenter
            left: parent.left
            right: parent.right
            leftMargin: item.lineWidth + 15
            rightMargin: item.lineWidth + 15
        }

        font {
            pointSize: item.width / 12
        }
    }
}
