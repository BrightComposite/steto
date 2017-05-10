import QtQuick 2.0

Item {
    id: button

    property string text: ""
    property bool enabled: true
    property bool pressed: button.enabled && mouseArea.pressed
    property int fontSize: 24

    signal triggered

    Rectangle {
        anchors.fill: parent

        radius: 4
        color: mouseArea.pressed ? "#555" : "#444"

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
        color: button.enabled ? "#a0ffffff" : "#20ffffff"

        font {
            pointSize: button.fontSize
        }

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
