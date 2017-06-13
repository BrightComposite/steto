import QtQuick 2.5
import QtGraphicalEffects 1.0

Item {
    id: chart

    property int minViewport: 0

    property var model: []
    property Component delegate

    property real range: 256

    function append(value) {
        model.push(value)
        update()
    }

    function update() {
        if(loader.delegate) {
            loader.delegate.update()
        }
    }

    onRangeChanged: {
        update()
    }

    onModelChanged: {
        update()
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#222"
        radius: 6
        //visible: false
    }
/*
    DropShadow {
        anchors.fill: background
        source: background
        samples: 33
        verticalOffset: 1
        color: "#30000000"
    }*/

    Item {
        id: viewport

        anchors {
            fill: background
            margins: 8
        }

        clip: true

        Loader {
            id: loader

            property var chart: chart
            property ChartDelegate delegate: item

            anchors.fill: parent
            sourceComponent: chart.delegate
            //visible: false
        }
/*
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
        }*/
/*
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
        }*/
    }
}
