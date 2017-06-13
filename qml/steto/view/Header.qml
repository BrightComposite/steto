import QtQuick 2.5
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.1

ToolBar {
    id: header

    Material.background: Material.color(Material.Grey, Material.Shade800)
    height: 54 * q

    RowLayout {
        anchors.centerIn: parent

        scale: q

        width: Math.ceil(parent.width / q) - 8
        height: Math.ceil(parent.height / q)

        ToolButton {
            id: back

            text: "<"

            font {
                pointSize: 24
            }

            height: parent.height
            width: height

            enabled: viewport.depth > 1
            opacity: enabled ? 1.0 : 0.0

            onClicked: viewport.pop()

            Behavior on opacity {
                OpacityAnimator {
                    duration: 200
                }
            }
        }

        Label {
            text: viewport.currentItem.title

            font {
                pointSize: 20
            }

            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        ToolButton {
            text: ""

            font {
                pointSize: 24
            }

            height: parent.height
            width: height

            onClicked: menu.open()

            Behavior on opacity {
                OpacityAnimator {
                    duration: 200
                }
            }

            Item {
                anchors.centerIn: parent

                width: 24
                height: 18

                Rectangle {
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }

                    height: 2
                }

                Rectangle {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: parent.right
                    }

                    height: 2
                }

                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    height: 2
                }
            }

            Menu {
                id: menu
                y: header.height

                MenuItem {
                    text: "Подключение"
                    enabled: viewport.currentItem && viewport.currentItem.title != "Подключение"

                    onTriggered: {
                        viewport.push(devicesView)
                    }
                }

                MenuItem {
                    text: "Новый пациент"
                    enabled: viewport.currentItem && viewport.currentItem.title != "Новый пациент"

                    onTriggered: {
                        viewport.push(createPatientView)
                    }
                }

                MenuItem {
                    text: "Внешние файлы"
                    enabled: viewport.currentItem && viewport.currentItem.title != "Внешние файлы"

                    onTriggered: {
                        viewport.push(externalFilesView)
                    }
                }

                MenuItem {
                    text: "Тест"
                    enabled: viewport.currentItem && viewport.currentItem.title != "Тест"

                    onTriggered: {
                        viewport.push(testAnalysisView)
                    }
                }
            }
        }
    }
}
