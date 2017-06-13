import QtQuick 2.5
import QtQuick.Layouts 1.1

import steto.view 1.0

Item {
    id: overlay

    property bool enabled: true

    property real movementSpeed: 0.125
    property int samplesCount: 128

    property int initialZoom: 0
    property int maxZoom: 2
    property int minZoom: -4

    property int dragMax: 0

    readonly property real minZoomFactor: zoomToScale(minZoom)
    readonly property real maxZoomFactor: zoomToScale(maxZoom)
    readonly property real zoomFactor: Math.max(minZoomFactor, Math.min(maxZoomFactor, pinched.currentScale))
    readonly property real samplePeriod: Math.min(zoomToScale(-minZoom), Math.max(zoomToScale(-maxZoom), 1 / pinched.currentScale))

    property bool dragEnabled: true
    property int  dragOffset: 0
        //Math.floor(dragMax * (1 - pinched.x / pinchDelta) / samplePeriod) * samplePeriod
        //Math.floor(dragHandle.x * dragMax / (dragArea.width - dragHandle.width) / samplePeriod) * samplePeriod

    readonly property int offset: Math.max(0, Math.min(pinchDelta, pinched.x))
    readonly property int pinchDelta: overlay.width * pinched.currentScale / movementSpeed

    Binding {
        target: overlay
        property: "dragOffset"
        value: Math.round(dragMax * (1 - offset / pinchDelta) / samplePeriod) * samplePeriod
        when: dragEnabled
    }

    function scaleToZoom(value) {
        return Math.round(Math.log(value) / Math.LN2)
    }

    function zoomToScale(value) {
        return value >= 0 ? (1 << value) : 1 / (1 << -value)
    }

    function discreteScale(value) {
        return zoomToScale(scaleToZoom(value))
    }

    onEnabledChanged: {
        if(enabled) {
            pinched.x = pinchDelta
        }
    }

    Item {
        id: pinched

        width: 1
        height: 1

        property real currentScale: initialZoom >= 0 ? (1 << initialZoom) : 1 / (1 << -initialZoom)
        scale: currentScale

        onScaleChanged: {
            dragEnabled = false
            var oldScale = currentScale
            currentScale = scale
            x = x * scale / oldScale
            dragOffset = Math.round(dragMax * (1 - offset / pinchDelta) / samplePeriod) * samplePeriod
            dragEnabled = true
        }

        Component.onCompleted: {
            x = pinchDelta
        }
    }

    PinchArea {
        id: pinchArea
        anchors.fill: parent

        pinch {
            target: pinched
            minimumScale: minZoomFactor
            maximumScale: maxZoomFactor
        }

        MouseArea {
            id: dragArea
            anchors.fill: parent
            scrollGestureEnabled: false

            propagateComposedEvents: true

            drag {
                target: pinched
                minimumX: -10
                maximumX: pinchDelta + 10
                axis: Drag.XAxis
            }
        }
    }

    Item {
        anchors {
            fill: parent
            margins: 8
        }

        visible: overlay.enabled

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

                    onTriggered: pinched.x = Math.min(pinchDelta, pinched.x + movementSpeed * (1 + pressDuration) * overlay.width / samplesCount)
                }

                LongPressButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: ">"

                    enabled: dragOffset < dragMax
                    maxPressDuration: 32

                    onTriggered: pinched.x = Math.max(0, pinched.x - movementSpeed * (1 + pressDuration) * overlay.width / samplesCount)
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

                    enabled: pinched.scale > overlay.minZoomFactor
                    onTriggered: pinched.scale /= 2
                }

                LongPressButton {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: "+"

                    enabled: pinched.scale < overlay.maxZoomFactor
                    onTriggered: pinched.scale *= 2
                }
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 3

            opacity: dragArea.pressed || pinchArea.pinch.active ? 0.8 : 0.0
            color: "#777"

            Rectangle {
                anchors {
                    bottom: parent.bottom
                }

                x: (parent.width - width) * dragOffset / dragMax
                width: parent.width * minZoomFactor / zoomFactor
                height: 3

                color: "#fff"

                radius: 1

                Behavior on opacity {
                    OpacityAnimator {
                        duration: 100
                    }
                }
            }
        }
    }
/*
    Item {
        id: dragArea

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 2
        }

        height: 1
        visible: false

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

            opacity: 0.8

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
    }*/
}
