import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Window 2.0

ApplicationWindow {
    id: window

    visible: true
    width: 480
    height: 640

    property real q: Math.min(window.width / 480, window.height / 640) * 3.7 / Screen.logicalPixelDensity
    property alias viewport: viewport

    header: Header {}

    StackView {
        id: viewport

        anchors.centerIn: parent
        initialItem: patientsView

        scale: q

        width: Math.ceil(parent.width / q)
        height: Math.ceil(parent.height / q)
    }
}
