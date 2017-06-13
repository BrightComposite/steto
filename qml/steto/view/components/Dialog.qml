import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1

Dialog {
    id: dialog

    modal: true

    property string text: ""

    x: (window.width / q - width) / 2
    y: (window.height / q - height) / 2

    width: Math.ceil(window.width / q) * 0.8
    height: column.height + 32

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
        }

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
