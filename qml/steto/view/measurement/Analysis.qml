import QtQuick 2.0
import QtQuick.Controls 2.1

import steto.view 1.0

MeasurementForm {
    id: page
    currentFile: patientsDir.currentFile

    onSelectColor: {
        colorDialog.area = index
        colorDialog.open()
    }

    Connections {
        target: provider

        onAreasChanged: {
            if(patientsDir.currentFile) {
                provider.serializeAreas(patientsDir.filePath)
            }
        }
    }

    Connections {
        target: provider.signal

        onSamplesChanged: {
            console.log("signal changed")
        }
    }

    ColorDialog {
        id: colorDialog

        colors: page.colors
        selected: provider.areas.length > 0 ? provider.areas[area].color : "#fff"

        property int area: 0

        delegate: Rectangle {
            width: 56
            height: 56

            color: Qt.rgba(modelData.r, modelData.g, modelData.b, modelData.a * 0.4)
            radius: 8

            border {
                width: 2
                color: modelData == colorDialog.selected ? "#fff" : Qt.rgba(modelData.r, modelData.g, modelData.b, modelData.a * 0.8)
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    colorDialog.selected = modelData
                }
            }
        }

        onAccepted: {
            provider.setAreaColor(area, selected)
        }
    }

    StackView.onActivated: {
        provider.updateAreas()
    }

    Component.onCompleted: {
        if(patientsDir.fileExists) {
            provider.unserialize(patientsDir.filePath)
        } else {
            console.log("Can't find file", patientsDir.filePath)
        }
    }
}
