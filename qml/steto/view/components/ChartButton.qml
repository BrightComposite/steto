import QtQuick 2.0

Item {
    id: button

    property string text: ""
    property bool enabled: true
    property int maxPressDuration: 8
    readonly property int pressDuration: mouseArea.pressDuration

    signal triggered

    Rectangle {
        anchors.fill: parent

        radius: 4
        color: mouseArea.pressed ? "#444" : "#333"

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
            pointSize: 24
        }

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: button.enabled

        property int pressDuration: 0

        Timer {
            id: timer
            repeat: true
            interval: 200

            running: button.enabled && mouseArea.pressed

            onTriggered: {
                button.triggered()

                if(pressDuration < maxPressDuration) {
                    ++pressDuration
                }
            }
        }

        onClicked: {
            if(button.enabled) {
                button.triggered()
            }
        }
    }
}
