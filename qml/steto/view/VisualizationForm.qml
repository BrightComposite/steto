import QtQuick 2.0
import QtQuick.Controls 2.1

Page {
    id: page
    title: "Исследование"

    signal start

    Item {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 16
        }

        Flickable {
            id: flickable
            anchors.fill: parent
            contentHeight: column.height

            Column {
                id: column

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    leftMargin: 16
                    rightMargin: 16
                }

                spacing: 32
/*
                Chart {
                    id: spectreChart

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 200
                    minViewport: 0

                    type: "spectre"
                    model: deviceService.isValid ? deviceService.data.spectre : [2.4, 1.3, 6.2, 1.3, 1.6, 0.6, 0.7, 0.2, 0.4, 3.6]
                    range: model.length > 0 ? model.reduce(function(max, e) { return Math.max(max, e) }) : 0
                }
*/
                SignalChart {
                    id: signalChart
                }

                MenuButton {
                    id: button

                    text: "Начать"

                    onClicked: {
                        page.start()
                    }

                    visible: deviceService.isConnected && !deviceService.data.active
                }

                ProgressRing {
                    id: ring

                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 160
                    height: 160

                    description: "прочитано"

                    visible: deviceService.isConnected && deviceService.data.active
                    progress: deviceService.data.progress
                }
            }
        }
    }
}
