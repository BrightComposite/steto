import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1

import ru.applabs 1.0

import steto.view 1.0
import steto.model 1.0

Page {
    id: page

    signal complete

    property string completeText: ""

    property Patient initial

    property Patient data: Patient {
        surname: surname.text
        name: name.text
        patronymic: patronymic.text
        birth_day: day.text
        birth_month: month.text
        birth_year: year.text
        anamnesis: anamnesis.text
    }

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

                spacing: 8

                Label {
                    text: "ФИО"

                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: 20

                    color: "#888"

                    font {
                        pointSize: 18
                    }
                }

                Input {
                    id: surname

                    anchors.horizontalCenter: parent.horizontalCenter

                    text: initial ? initial.surname : ""
                    placeholderText: "Фамилия"
                    Material.accent: displayText.length > 0 ? Material.Green : Material.Red
                }

                Input {
                    id: name
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: initial ? initial.name : ""
                    placeholderText: "Имя"
                    Material.accent: displayText.length > 0 ? Material.Green : Material.Red
                }

                Input {
                    id: patronymic
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: initial ? initial.patronymic : ""
                    placeholderText: "Отчество"
                    Material.accent: displayText.length > 0 ? Material.Green : Material.Red
                }

                Item { width: 1; height: 1 }

                Label {
                    text: "Дата рождения"

                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: 24

                    color: "#888"

                    font {
                        pointSize: 18
                    }
                }

                Item {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: 50

                    GridLayout {
                        anchors.fill: parent
                        columnSpacing: 8

                        rows: 1
                        columns: 4

                        Input {
                            id: day
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.columnSpan: 1

                            text: initial ? initial.birth_day : ""
                            placeholderText: "День"

                            validator: IntValidator { bottom: 1; top: month.text && year.text ? Utils.daysInMonth(month.text, year.text) : 31 }
                            inputMethodHints: Qt.ImhDigitsOnly
                            Material.accent: acceptableInput ? Material.Green : Material.Red
                        }

                        Input {
                            id: month
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.columnSpan: 1

                            text: initial ? initial.birth_month : ""
                            placeholderText: "Месяц"

                            validator: IntValidator { bottom: 1; top: 12 }
                            inputMethodHints: Qt.ImhDigitsOnly
                            Material.accent: acceptableInput ? Material.Green : Material.Red
                        }

                        Input {
                            id: year
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.columnSpan: 2

                            text: initial ? initial.birth_year : ""
                            placeholderText: "Год"

                            validator: IntValidator { bottom: 1900; top: Utils.date().getFullYear() }
                            inputMethodHints: Qt.ImhDigitsOnly
                            Material.accent: acceptableInput ? Material.Green : Material.Red
                        }
                    }

                }

                Item { width: 1; height: 1 }

                Label {
                    text: "Анамнез"

                    anchors.horizontalCenter: parent.horizontalCenter

                    width: 300
                    height: 20

                    color: "#888"

                    font {
                        pointSize: 18
                    }
                }

                TextArea {
                    id: anamnesis
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: initial ? initial.anamnesis : ""
                    placeholderText: "Заметки о пациенте,\nдиагноз"
                }

                Item { width: 1; height: 1 }

                MenuButton {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: completeText

                    font {
                        pointSize: 14
                    }

                    enabled:
                        surname.text.length > 0 &&
                        name.text.length > 0 &&
                        patronymic.text.length > 0 &&
                        year.acceptableInput &&
                        month.acceptableInput &&
                        day.acceptableInput

                    onClicked: page.complete()
                }

                Item { width: 1; height: 16 }
            }
        }
    }
}
