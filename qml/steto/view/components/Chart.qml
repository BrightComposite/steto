import QtQuick 2.5
import QtGraphicalEffects 1.0

import "chart"

Item {
    id: chart

    property int minViewport: 0

    property string type: ""
    property var model: []

    property real range: 256

    function append(value) {
        model.push(value)
        update()
    }

    function update() {
        if(loader.item) {
            loader.item.requestPaint()
        }
    }

    onModelChanged: {
        update()
    }

    Component { id: signalType; Signal {} }
    Component { id: spectrogramType; Spectrogram {} }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#222"
        radius: 6
        visible: false
    }

    DropShadow {
        anchors.fill: background
        source: background
        samples: 33
        verticalOffset: 1
        color: "#30000000"
    }

    Item {
        id: viewport

        anchors {
            fill: background
            margins: 8
        }

        clip: true

        Loader {
            id: loader

            anchors {
                fill: parent
            }

            sourceComponent: {
                switch(chart.type) {
                    case "spectre":
                        return spectrogramType
                    case "signal":
                    default:
                        return signalType
                }
            }

            onLoaded: {
                item.requestPaint()
            }

            visible: false
        }

        Rectangle {
            id: mask
            anchors.fill: loader
            radius: 4
            color: "#fff"

            visible: false
        }

        OpacityMask {
            anchors.fill: loader
            source: loader
            maskSource: mask
        }

        Rectangle {
            id: glare

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            height: parent.height * 0.4
            radius: 4

            gradient: Gradient {
                GradientStop { position: 0.0; color: "#1fffffff" }
                GradientStop { position: 1.0; color: "#00ffffff" }
            }
        }
    }
}
