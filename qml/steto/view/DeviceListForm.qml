import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

import steto 1.0
import steto.view 1.0

Page {
    id: page

    title: "Устройство"

    property var model: []

    signal search
    signal select(string address)

    Item {
        id: container

        anchors {
            top: parent.top
            bottom: button.top
            horizontalCenter: parent.horizontalCenter
            margins: 20
        }

        width: 250

        Flickable {
            id: flickable

            anchors.fill: parent
            contentHeight: column.height

            clip: true

            Column {
                id: column

                anchors {
                    left: parent.left
                    right: parent.right
                }

                spacing: 8

                Repeater {
                    model: page.model

                    Button {
                        id: device
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: column.width
                        height: 100

                        Material.background: Material.color(Material.Grey, Material.Shade900)

                        enabled: !deviceService.isConnecting
                        visible: deviceService.currentDevice == null || modelData.address == deviceService.currentDevice.address

                        contentItem: Item {
                            anchors {
                                fill: parent
                            }

                            RowLayout {
                                anchors {
                                    fill: parent
                                    leftMargin: 16
                                    rightMargin: 16
                                }

                                Column {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter

                                    spacing: 4

                                    Text {
                                        anchors {
                                            left: parent.left
                                            right: parent.right
                                        }

                                        text: modelData.name
                                        color: "#fff"

                                        font {
                                            pointSize: 24
                                        }

                                        elide: Label.ElideRight
                                    }

                                    Text {
                                        anchors {
                                            left: parent.left
                                            right: parent.right
                                        }

                                        text: modelData.address
                                        color: "#aaa"

                                        font {
                                            pointSize: 10
                                        }

                                        elide: Label.ElideRight
                                    }
                                }

                                CheckBox {
                                    Layout.preferredWidth: 40
                                    Layout.preferredHeight: 40
                                    Layout.alignment: Qt.AlignVCenter

                                    text: ""
                                    checked: deviceService.currentDevice && (modelData.address == deviceService.currentDevice.address) && deviceService.isConnected
                                    enabled: false
                                }
                            }
                        }

                        onClicked: {
                            if(!deviceService.currentDevice || modelData.address != deviceService.currentDevice.address) {
                                page.select(modelData.address)
                            } else {
                                page.select("0")
                            }
                        }
                    }
                }
            }
        }

        ListShadow {
            anchors.fill: parent
            list: flickable
        }
    }

    Text {
        anchors.centerIn: container
        width: container.width

        text: "Нет доступных устройств"
        color: "#ccc"

        font {
            pointSize: 20
        }

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        visible: page.model.length == 0
    }

    MenuButton {
        id: button

        anchors {
            bottom: parent.bottom
            bottomMargin: 60
        }

        text: "Поиск"

        onClicked: {
            page.search()
        }
    }
}
