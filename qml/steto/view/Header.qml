import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.1

ToolBar {
    Material.background: Material.color(Material.Grey, Material.Shade800)
    height: 54

    RowLayout {
        anchors.centerIn: parent

        scale: q

        width: Math.ceil(parent.width / q) - 16
        height: Math.ceil(parent.height / q)

        ToolButton {
            id: back

            text: "<"

            font {
                pointSize: 24
            }

            height: parent.height
            width: height

            visible: viewport.depth > 0
            onClicked: viewport.pop()
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
            text: "⋮"

            font {
                pointSize: 24
            }

            height: parent.height
            width: height

            //onClicked: menu.open()
        }
    }
}
