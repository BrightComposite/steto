import QtQuick 2.0
import QtQuick.Controls 2.1 as Controls

Controls.TextArea {
    width: 300
    height: Math.max(contentHeight + 32, 80)

    font {
        pointSize: 18
    }
}
