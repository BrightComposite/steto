import QtQuick 2.5
import QtQuick.Controls 2.1

import steto.view 1.0

Page {
    id: page

    signal edit
    signal remove
    signal newMeasurement
    signal selectFile(string filename)

    Item {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 32
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
                }

                spacing: 16

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: 32

                    color: "#fff"
                    text: patients.currentPatient ?
                            patients.currentPatient.surname + " " +
                            patients.currentPatient.name + " " +
                            patients.currentPatient.patronymic : ""

                    font {
                        pointSize: 18
                    }

                    horizontalAlignment: Qt.AlignHCenter
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: 32

                    color: "#fff"
                    text: patients.currentPatient ? patients.currentPatient.displayBirthday() : ""

                    font {
                        pointSize: 18
                    }

                    horizontalAlignment: Qt.AlignHCenter
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: Math.max(50, contentHeight)

                    color: "#a0ffffff"
                    text: patients.currentPatient ? patients.currentPatient.anamnesis : ""

                    font {
                        pointSize: 14
                    }

                    visible: patients.currentPatient ? patients.currentPatient.anamnesis : ""
                }

                Item {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 200
                    height: 70

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: parent.height

                        spacing: 8

                        MenuButton {
                            width: 160

                            text: "Редактировать"

                            font {
                                pointSize: 14
                            }

                            onClicked: page.edit()
                        }

                        MenuButton {
                            width: 160

                            text: "Удалить"

                            font {
                                pointSize: 14
                            }

                            onClicked: page.remove()
                        }
                    }
                }

                MenuButton {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Новое исследование"

                    font {
                        pointSize: 14
                    }

                    enabled: deviceService.isConnected

                    onClicked: page.newMeasurement()
                }

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    color: "#80ffffff"
                    text: "Необходимо подключить устройство"

                    visible: !deviceService.isConnected

                    font {
                        pointSize: 14
                    }

                    horizontalAlignment: Qt.AlignHCenter
                }

                Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: 20
                    }

                    height: 1
                    color: "#10ffffff"
                }

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    color: "#a0ffffff"
                    text: "Сохраненные исследования:"

                    font {
                        pointSize: 14
                    }

                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                }

                Item { width: 1; height: 1 }

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    color: "#80ffffff"
                    text: "Нет сохраненных исследований"

                    font {
                        pointSize: 20
                    }

                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter

                    visible: patientsDir.files.length == 0
                }

                Repeater {
                    model: patientsDir.files

                    FlatButton {
                        anchors.horizontalCenter: parent.horizontalCenter

                        width: 300
                        height: 64

                        text: modelData
                        fontSize: 18

                        onTriggered: page.selectFile(modelData)

                        Rectangle {
                            anchors.fill: parent
                            color: "#20ffffff"
                            radius: 4

                            opacity: patientsDir.currentFile != modelData ? 1.0 : 0.0

                            Behavior on opacity {
                                OpacityAnimator {
                                    duration: 200
                                }
                            }
                        }
                    }
                }

                Item { width: 1; height: 16 }
            }
        }
    }
}
