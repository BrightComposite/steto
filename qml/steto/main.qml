import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.0

import steto 1.0
import steto.view 1.0

ApplicationWindow {
    id: window

    visible: true
    width: 480
    height: 640
    title: "Стетоскоп"

    property alias viewport: viewport

    property Component mainMenu: Component { MainMenu {} }
    property Component deviceList: Component { DeviceList {} }
    property Component visualization: Component { Visualization {} }

    property real q: Math.min(window.width / 480, window.height / 640) * 3.7 / Screen.logicalPixelDensity

    DeviceService {
        id: deviceService
    }

    header: Header {}

    StackView {
        id: viewport

        anchors.centerIn: parent
        initialItem: mainMenu

        scale: q

        width: Math.ceil(parent.width / q)
        height: Math.ceil(parent.height / q)
    }
}
