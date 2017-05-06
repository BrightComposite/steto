import QtQuick 2.0
import QtQuick.Controls 2.1
import QtCharts 2.0

Page {
    id: page
    title: "Исследование"

    signal toggle

    MenuButton {
        anchors {
            top: parent.top
            topMargin: 16
        }

        text: deviceService.data.active ? "Остановить" : "Начать"
        onClicked: {
            page.toggle()
        }
    }
}
