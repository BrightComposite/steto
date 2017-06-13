import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1

import steto 1.0
import steto.view 1.0

Page {
    id: page
    title: "Исследование"

    property bool serializable: false
    property bool recording: false
    property bool ready: false
    property bool interactive: true
    property bool editable: true

    property real progress: 0

    property SampleProvider provider: SampleProvider {}
    property string currentFile: ""

    property var signal: provider.signal.samples
    property var spectre: provider.spectre.samples

    property var colors: [
        Material.color(Material.Red, Material.Shade200),
        Material.color(Material.Pink, Material.Shade200),
        Material.color(Material.Purple, Material.Shade200),
        Material.color(Material.DeepPurple, Material.Shade200),

        Material.color(Material.Indigo, Material.Shade200),
        Material.color(Material.Blue, Material.Shade200),
        Material.color(Material.LightBlue, Material.Shade200),
        Material.color(Material.Cyan, Material.Shade200),

        Material.color(Material.Teal, Material.Shade200),
        Material.color(Material.Green, Material.Shade200),
        Material.color(Material.LightGreen, Material.Shade200),
        Material.color(Material.Lime, Material.Shade200),

        Material.color(Material.Yellow, Material.Shade200),
        Material.color(Material.Amber, Material.Shade200),
        Material.color(Material.Orange, Material.Shade200),
        Material.color(Material.DeepOrange, Material.Shade200)
    ]

    signal start
    signal cancel
    signal save
    signal clear
    signal selectColor(int index)

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

            MouseArea {
                anchors.fill: parent
                focus: true
                preventStealing: true
                propagateComposedEvents: true
                scrollGestureEnabled: false
                onClicked: forceActiveFocus()
            }

            Column {
                id: column

                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 16
                    rightMargin: 16
                }

                spacing: 16

                Item {
                    id: signalContainer

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    property bool opened: true

                    height: opened ? 200 : 40

                    Behavior on height {
                        NumberAnimation {
                            duration: 200
                        }
                    }

                    Chart {
                        id: signalChart

                        anchors.fill: parent
                        minViewport: 128

                        model: page.signal
                        delegate: Signal {}

                        range: provider.signal.max * 2

                        opacity: signalContainer.opened ? 1.0 : 0.0

                        Behavior on opacity {
                            OpacityAnimator {
                                duration: 200
                            }
                        }

                        ChartOverlay {
                            id: signalOverlay
                            anchors.fill: parent

                            initialZoom: -4
                            maxZoom: 0
                            minZoom: -8
                            dragMax: page.interactive ? Math.max(0, provider.signal.total - samplesCount * samplePeriod) : 0
                            enabled: page.interactive && signalContainer.opened

                            Binding {
                                target: provider.signal
                                property: "offset"
                                value: signalOverlay.dragOffset
                            }

                            Binding {
                                target: provider.signal
                                property: "count"
                                value: signalOverlay.samplesCount
                            }

                            Binding {
                                target: provider.signal
                                property: "period"
                                value: signalOverlay.samplePeriod
                            }
                        }
                    }

                    FlatButton {
                        anchors {
                            top: parent.top
                            horizontalCenter: parent.horizontalCenter
                            topMargin: 8
                        }

                        text: signalContainer.opened ? "Скрыть" : "Сигнал"
                        fontSize: 20

                        width: 100
                        height: 32

                        onTriggered: {
                            signalContainer.opened ^= 1
                        }
                    }
                }

                Chart {
                    id: spectreChart

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 200
                    minViewport: 0

                    model: page.spectre
                    delegate: Spectrogram {
                        offset: spectreOverlay.dragOffset * provider.frequencyStep
                        step: provider.frequencyStep * Math.max(1, Math.floor(provider.spectre.period))
                        areas: provider.areas
                    }

                    range: Math.max.apply(Math, page.spectre)

                    Behavior on range {
                        NumberAnimation {
                            duration: 200
                        }
                    }

                    ChartOverlay {
                        id: spectreOverlay
                        anchors.fill: parent

                        maxZoom: 3
                        minZoom: 7 - spectreOverlay.scaleToZoom(provider.spectre.total)

                        movementSpeed: 1

                        dragMax: page.interactive ? Math.max(0, provider.spectre.total - samplesCount * samplePeriod) : 0
                        enabled: page.interactive

                        Binding {
                            target: provider.spectre
                            property: "offset"
                            value: spectreOverlay.dragOffset
                        }

                        Binding {
                            target: provider.spectre
                            property: "count"
                            value: spectreOverlay.samplesCount
                        }

                        Binding {
                            target: provider.spectre
                            property: "period"
                            value: spectreOverlay.samplePeriod
                        }
                    }
                }

                Item { width: 1; height: 1 }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 70

                    spacing: 8

                    MenuButton {
                        width: 160
                        text: "Сохранить"

                        onClicked: page.save()

                        visible: serializable
                    }

                    MenuButton {
                        width: 160
                        text: "Сбросить"

                        onClicked: page.clear() - offset

                        visible: serializable
                    }
                }

                MenuButton {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Начать"

                    font {
                        pointSize: 14
                    }

                    onClicked: page.start()

                    visible: page.ready && !page.interactive
                }

                ProgressRing {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 160
                    height: 160

                    description: "прочитано"

                    visible: page.recording
                    progress: page.progress
                }

                MenuButton {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Отменить"

                    font {
                        pointSize: 14
                    }

                    onClicked: page.cancel()

                    visible: page.recording && !page.interactive
                }

                Repeater {
                    model: provider.areas
                    visible: page.editable

                    Item {
                        anchors.horizontalCenter: parent.horizontalCenter

                        width: 200
                        height: 50

                        RowLayout {
                            anchors.fill: parent

                            Input {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                text: modelData.start
                                placeholderText: "Начало"

                                validator: IntValidator { bottom: SampleProvider.MIN_FREQUENCY; top: SampleProvider.MAX_FREQUENCY }
                                inputMethodHints: Qt.ImhDigitsOnly
                                Material.accent: acceptableInput ? Material.Green : Material.Red

                                horizontalAlignment: Qt.AlignHCenter

                                onEditingFinished: {
                                    provider.setAreaStart(index, text)
                                }

                                Text {
                                    x: (parent.width + parent.contentWidth) / 2 + 4
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        verticalCenterOffset: -3
                                    }

                                    color: "#10ffffff"
                                    text: "Hz"
                                    font: parent.font
                                }
                            }

                            Input {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                text: modelData.end
                                placeholderText: "Конец"

                                validator: IntValidator { bottom: SampleProvider.MIN_FREQUENCY; top: SampleProvider.MAX_FREQUENCY }
                                inputMethodHints: Qt.ImhDigitsOnly
                                Material.accent: acceptableInput ? Material.Green : Material.Red

                                horizontalAlignment: Qt.AlignHCenter

                                onEditingFinished: {
                                    provider.setAreaEnd(index, text)
                                }

                                Text {
                                    x: (parent.width + parent.contentWidth) / 2 + 4
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        verticalCenterOffset: -3
                                    }

                                    color: "#10ffffff"
                                    text: "Hz"
                                    font: parent.font
                                }
                            }
                        }

                        Rectangle {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                right: parent.left
                                rightMargin: 16
                            }

                            width: 32
                            height: 32

                            property color c: modelData.color

                            color: Qt.rgba(c.r, c.g, c.b, c.a * 0.4)
                            radius: 8

                            border {
                                width: 2
                                color: Qt.rgba(c.r, c.g, c.b, c.a * 0.8)
                            }

                            MouseArea {
                                anchors {
                                    fill: parent
                                    margins: -20
                                }

                                onClicked: page.selectColor(index)
                            }
                        }

                        FlatButton {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                left: parent.right
                                leftMargin: 16
                            }

                            width: 32
                            height: 32

                            text: "×"
                            fontSize: 28

                            onTriggered: provider.removeArea(index)
                        }
                    }
                }

                MenuButton {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Добавить область"

                    font {
                        pointSize: 14
                    }

                    readonly property int start: spectreOverlay.dragOffset * provider.frequencyStep

                    onClicked: {
                        provider.addArea(
                            start,
                            start + provider.spectre.samples.length * provider.frequencyStep * Math.max(1, Math.floor(provider.spectre.period)),
                            page.colors[Math.round(Math.random() * (page.colors.length - 1))]
                        )
                    }

                    visible: page.editable
                }

                Item { width: 1; height: 16 }
            }
        }
    }
}
