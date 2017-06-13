import QtQuick 2.5
import QtQuick.Controls 2.1

import steto.view 1.0

Page {
    id: page

    signal selectFile(string filename)

    Item {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 32
        }

        Flickable {
            id: flickable
            anchors.fill: parent
            contentHeight: column.height

            Column {
                id: column

                anchors {
                    left: parent.left
                    right: parent.right
                }

                spacing: 16

                Repeater {
                    model: wavDir.files

                    FlatButton {
                        anchors.horizontalCenter: parent.horizontalCenter

                        width: 300
                        height: 64

                        text: modelData
                        fontSize: 18

                        onTriggered: page.selectFile(modelData)

                        Rectangle {
                            anchors.fill: parent
                            color: "#20ffffff"
                            radius: 4

                            opacity: wavDir.currentFile != modelData ? 1.0 : 0.0

                            Behavior on opacity {
                                OpacityAnimator {
                                    duration: 200
                                }
                            }
                        }
                    }
                }

                Item { width: 1; height: 16 }
            }
        }
    }
}
