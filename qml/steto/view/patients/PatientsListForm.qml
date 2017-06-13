import QtQuick 2.5
import QtQuick.Controls 2.1

import steto.view 1.0

Page {
    id: page

    signal addPatient
    signal selectPatient(var patient)

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
                    leftMargin: 16
                    rightMargin: 16
                }

                spacing: 16

                Repeater {
                    function map(records) {
                        var lastChar = null

                        return records.map(function(e) {
                            var isFirst = false

                            if(e.surname && lastChar != e.surname[0]) {
                                lastChar = e.surname[0]
                                isFirst = true
                            }

                            return {
                                isFirst: isFirst,
                                patient: e
                            }
                        })
                    }

                    model: map(patients.records)

                    PatientItem {
                        width: 250
                        height: 64

                        text: modelData.patient.surname + " " + modelData.patient.name
                        isFirst: modelData.isFirst

                        onTriggered: page.selectPatient(modelData.patient)
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Нет пациентов"
                    color: "#ccc"

                    font {
                        pointSize: 20
                    }

                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter

                    visible: patients.records.length == 0
                }

                Item { width: 1; height: 1 }

                MenuButton {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Добавить пациента"

                    font {
                        pointSize: 14
                    }

                    onClicked: page.addPatient()
                }

                Item { width: 1; height: 16 }
            }
        }
    }
}
