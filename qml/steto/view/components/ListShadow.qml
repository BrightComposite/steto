import QtQuick 2.0

Item {
    anchors.fill: parent

    property Flickable list
    property int threshold: 0

    Item {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        SideShadow {
            side: Qt.AlignBottom
            opacity: list.contentY > threshold ? 1.0 : 0.0
        }
    }

    Item {
        anchors {
            top: parent.bottom
            left: parent.left
            right: parent.right
        }

        SideShadow {
            side: Qt.AlignTop
            opacity: list.contentY < list.contentHeight - list.height - threshold ? 1.0 : 0.0
        }
    }
}
