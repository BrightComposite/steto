import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1

Dialog {
    id: dialog

    modal: true

    property string text: "Выберите цвет"

    x: (window.width - width) / 2
    y: (window.height - height) / 2

    width: window.width * 0.8
    height: column.height + 32

    property var colors: []
    property Component delegate

    property color selected

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
        }

        spacing: 16

        Text {
            anchors {
                left: parent.left
                right: parent.right
            }

            color: "#fff"
            text: dialog.text

            font {
                pointSize: 18
            }

            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
        }

        Grid {
            anchors.horizontalCenter: parent.horizontalCenter

            columns: 4
            rowSpacing: 8
            columnSpacing: 8

            Repeater {
                model: dialog.colors
                delegate: dialog.delegate
            }
        }

        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
            }

            height: 70
            spacing: 8

            MenuButton {
                Layout.fillWidth: true
                text: "Ок"
                onClicked: dialog.accept()
            }

            MenuButton {
                Layout.fillWidth: true
                text: "Отмена"
                onClicked: dialog.close()
            }
        }
    }
}
