import QtQuick 2.7

Rectangle {
    property int side: Qt.AlignBottom
    property int size: 12
    property int offset: 0
    property color origin: "#30000000"

    anchors {
        verticalCenterOffset: (side == Qt.AlignTop) ? -(parent.height + offset + size - 2) / 2 : (side == Qt.AlignBottom) ? (parent.height + offset + size - 1) / 2 : 0
        horizontalCenterOffset: (side == Qt.AlignLeft) ? -(parent.width + offset + size) / 2 : (side == Qt.AlignRight) ? (parent.width + offset + size - 1) / 2 : 0
    }

    width: (side == Qt.AlignLeft || side == Qt.AlignRight) ? parent.height : parent.width
    height: size

    anchors.centerIn: parent
    rotation: (side == Qt.AlignLeft) ? 90: (side == Qt.AlignTop) ? 180: (side == Qt.AlignRight) ? -90 : 0

    gradient: Gradient {
        GradientStop {
            id: start
            position: 0
            color: origin
        }

        GradientStop {
            id: end
            position: 1.0
            color: "#00000000"
        }
    }

    Behavior on opacity {
        OpacityAnimator {
            duration: 250
        }
    }
}
