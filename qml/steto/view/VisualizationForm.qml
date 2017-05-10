import QtQuick 2.5
import QtQuick.Controls 2.1

import steto 1.0

Page {
    id: page
    title: "Исследование"

    signal start

    property bool completed: deviceService.data.count == DataService.SIZE

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
                    left: parent.left
                    right: parent.right
                    leftMargin: 16
                    rightMargin: 16
                }

                spacing: 16

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

                SignalChart {
                    id: signalChart

                    completed: page.completed
                    model: deviceService.isValid ? deviceService.data.displayed : [125, 31, 35, -74, -122, 64, 34, -64, -34, 34, 64, 23, -24, -64, -74]

                    minViewport: deviceService.isValid ? completed ? samplesCount : 128 : 0

                    onDragOffsetChanged: updateView()
                    onSamplesCountChanged: updateView()
                    onSamplePeriodChanged: updateView()
                    onCompletedChanged: updateView()

                    function updateView() {
                        deviceService.data.setDisplayRange(dragOffset, samplesCount, samplePeriod)
                    }
                }

                Item {
                    width: 1
                    height: 1
                }

                MenuButton {
                    text: "Сохранить"

                    onClicked: {
                        var date = new Date
                        patientsService.currentFile = date.toISOString().replace(/[\:\.]/g, "-") + ".txt"

                        if(deviceService.data.serialize(patientsService.filePath)) {
                            patientsService.update()
                        } else {
                            patientsService.currentFile = ""
                        }
                    }

                    visible: page.completed && patientsService.currentFile == ""
                }

                MenuButton {
                    text: "Новое исследование"

                    onClicked: {
                        patientsService.currentFile = ""
                        page.start()
                    }

                    visible: deviceService.isConnected && !deviceService.data.active
                }

                ProgressRing {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 160
                    height: 160

                    description: "прочитано"

                    visible: deviceService.isConnected && deviceService.data.active
                    progress: deviceService.data.progress
                }

                Item { width: 1; height: 1 }

                Repeater {
                    model: patientsService.files

                    FlatButton {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height: 64

                        text: modelData
                        fontSize: 18

                        visible: !deviceService.data.active

                        onTriggered: {
                            patientsService.currentFile = modelData

                            if(patientsService.fileExists) {
                                deviceService.data.unserialize(patientsService.filePath)
                                signalChart.updateView()
                            }
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "#20ffffff"
                            radius: 4

                            opacity: patientsService.currentFile == modelData ? 1.0 : 0.0

                            Behavior on opacity {
                                OpacityAnimator {
                                    duration: 200
                                }
                            }
                        }
                    }
                }

                Item { width: 1; height: 1 }
            }
        }
    }

    Component.onCompleted: {
        if(patientsService.fileExists) {
            deviceService.data.unserialize(patientsService.filePath)
            signalChart.updateView()
        }
    }
}
