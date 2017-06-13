import QtQuick 2.0

Item {
    id: button

    property string text: ""
    property bool enabled: true
    property bool pressed: button.enabled && mouseArea.pressed
    property bool isFirst: false
    property int fontSize: 18

    signal triggered

    anchors.horizontalCenter: parent.horizontalCenter

    Text {
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.left
            rightMargin: (window.width - button.width) * 0.2
        }

        color: "#80ffffff"
        text: button.text.length > 0 ? button.text[0] : ""
        visible: isFirst

        font {
            pointSize: button.fontSize * 1.5
        }
    }

    Rectangle {
        anchors.fill: parent

        radius: 2
        color: mouseArea.pressed ? "#555" : "#888"

        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }

        border {
            width: 1
            color: "#222"
        }

        opacity: 0.2
    }

    Text {
        anchors.fill: parent

        text: button.text
        color: button.enabled ? "#fff" : "#a0ffffff"

        font {
            pointSize: button.fontSize
        }

        elide: Text.ElideRight
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: button.enabled

        onClicked: {
            if(button.enabled) {
                button.triggered()
            }
        }
    }
}
