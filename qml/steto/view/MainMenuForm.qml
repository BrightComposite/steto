import QtQuick 2.0
import QtQuick.Controls 2.1

import steto.view 1.0

Page {
    title: "Главное меню"

    Column {
        anchors.centerIn: parent

        MenuButton {
            text: "Пациенты"
        }

        MenuButton {
            text: "Устройство"

            onClicked: {
                viewport.push(deviceList)
            }
        }

        MenuButton {
            text: "Пациенты"
            visible: deviceService.connected

            onClicked: {
                viewport.push(visualization)
            }
        }
    }
}
