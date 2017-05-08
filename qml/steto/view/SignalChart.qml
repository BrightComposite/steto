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

    readonly property bool completed: deviceService.data.count == DataService.SIZE

    property int movementSpeed: 8
    property int samplesCount: 128
    property int zoom: maxZoom - 3
    property int dragMax: completed ? DataService.SIZE - samplesCount * samplePeriod : 0

    readonly property int maxZoom: 8
    readonly property int samplePeriod: zoom <= 0 ? 1 << maxZoom : zoom < maxZoom ? 1 << (maxZoom - zoom) : 1
    readonly property int dragOffset: dragHandle.x * dragMax / (dragArea.width - dragHandle.width)

    minViewport: deviceService.isValid ? completed ? samplesCount : 160 : 0
    model: deviceService.isValid ? deviceService.data.displayed : [125, 31, 35, -74, -122, 64, 34, -64, -34, 34, 64, 23, -24, -64, -74]

    function setDragOffset(value) {
        if(value < 0) {
            value = 0
        } else if(value > dragMax) {
            value = dragMax
        }

        dragHandle.x = value * (dragArea.width - dragHandle.width) / dragMax
    }

    onDragOffsetChanged: {
        deviceService.data.setDisplayRange(dragOffset, samplesCount, samplePeriod)
    }

    onSamplesCountChanged: {
        deviceService.data.setDisplayRange(dragOffset, samplesCount, samplePeriod)
    }

    onSamplePeriodChanged: {
        deviceService.data.setDisplayRange(dragOffset, samplesCount, samplePeriod)
    }

    onCompletedChanged: {
        if(completed) {
            console.log("Completed")
            setDragOffset(dragMax)
        }
    }

    Item {
        anchors {
            fill: parent
            margins: 8
        }

        visible: deviceService.data.count == DataService.SIZE

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

                ChartButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: "<"

                    enabled: dragOffset > 0
                    maxPressDuration: 16

                    onTriggered: setDragOffset(dragOffset - movementSpeed * samplePeriod * (1 + pressDuration))
                }

                ChartButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: ">"

                    enabled: dragOffset < dragMax
                    maxPressDuration: 16

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

                ChartButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: "-"

                    enabled: chart.zoom > 0
                    onTriggered: --zoom
                }

                ChartButton {
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
