import QtQuick 2.0

FlatButton {
    id: button

    property int maxPressDuration: 8
    readonly property int pressDuration: timer.pressDuration

    Timer {
        id: timer
        repeat: true
        interval: 200

        property int pressDuration: 0

        running: button.pressed

        onTriggered: {
            button.triggered()

            if(pressDuration < maxPressDuration) {
                ++pressDuration
            }
        }

        onRunningChanged: {
            pressDuration = 0
        }
    }
}
