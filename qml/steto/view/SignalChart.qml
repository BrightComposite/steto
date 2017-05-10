import QtQuick 2.0
import QtQuick.Layouts 1.1

import steto 1.0

Chart {
    id: chart

    anchors {
        left: parent.left
        right: parent.right
    }

    height: 200

    property bool completed: false

    property int movementSpeed: 8
    property int samplesCount: 128
    property int zoom: maxZoom - 3
    property int dragMax: completed ? DataService.SIZE - samplesCount * samplePeriod : 0

    readonly property int maxZoom: 8
    readonly property int samplePeriod: zoom <= 0 ? 1 << maxZoom : zoom < maxZoom ? 1 << (maxZoom - zoom) : 1
    readonly property int dragOffset: Math.floor(dragHandle.x * dragMax / (dragArea.width - dragHandle.width) / samplePeriod) * samplePeriod

    function setDragOffset(value) {
        if(value < 0) {
            value = 0
        } else if(value > dragMax) {
            value = dragMax
        }

        dragHandle.x = dragMax > 0 ? value * (dragArea.width - dragHandle.width) / dragMax : 0
    }

    onCompletedChanged: {
        if(completed) {
            setDragOffset(0)
        }
    }

    Item {
        anchors {
            fill: parent
            margins: 8
        }

        visible: completed

        Item {
            anchors {
                top: parent.top
                left: parent.left
                margins: 4
            }

            width: 80
            height: 32

            RowLayout {
                anchors.fill: parent

                LongPressButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: "<"

                    enabled: dragOffset > 0
                    maxPressDuration: 32

                    onTriggered: setDragOffset(dragOffset - movementSpeed * samplePeriod * (1 + pressDuration))
                }

                LongPressButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: ">"

                    enabled: dragOffset < dragMax
                    maxPressDuration: 32

                    onTriggered: setDragOffset(dragOffset + movementSpeed * samplePeriod * (1 + pressDuration))
                }
            }
        }

        Item {
            anchors {
                top: parent.top
                right: parent.right
                margins: 4
            }

            width: 80
            height: 32

            RowLayout {
                anchors.fill: parent

                LongPressButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: "-"

                    enabled: chart.zoom > 0
                    onTriggered: --zoom
                }

                LongPressButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: "+"

                    enabled: chart.zoom < chart.maxZoom
                    onTriggered: ++zoom
                }
            }
        }

        Item {
            id: dragArea

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 1

            Rectangle {
                id: dragHandle

                anchors.bottom: parent.bottom
                visible: dragMax > 0

                width: 64
                height: 16

                radius: 4
                color: mouseArea.pressedButtons != 0 ? "#444" : "#333"

                border {
                    width: 1
                    color: "#222"
                }

                Component.onCompleted: {
                    dragHandle.x = 0
                }

                MouseArea {
                    id: mouseArea

                    anchors {
                        fill: parent
                        margins: -32
                    }

                    drag {
                        target: dragHandle
                        axis: Drag.XAxis
                        minimumX: 0
                        maximumX: dragArea.width - dragHandle.width
                    }
                }
            }
        }
    }
}
