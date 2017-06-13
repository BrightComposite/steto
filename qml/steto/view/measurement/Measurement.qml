import QtQuick 2.0

import steto 1.0
import steto.view 1.0

MeasurementForm {
    id: page

    ready: deviceService.isConnected && !deviceService.recording.isActive
    recording: deviceService.isConnected && deviceService.recording.isActive
    interactive: deviceService.recording.isCompleted
    serializable: deviceService.recording.isCompleted && currentFile == ""
    editable: deviceService.recording.isCompleted

    provider: deviceService.recording.provider

    progress: deviceService.recording.progress

    currentFile: patientsDir.currentFile

    onSelectColor: {
        colorDialog.area = index
        colorDialog.open()
    }

    onStart: {
        patientsDir.currentFile = ""
        deviceService.recording.start()
    }

    onCancel: {
        deviceService.recording.stop()
    }

    Connections {
        target: provider

        onAreasChanged: {
            if(patientsDir.currentFile) {
                provider.serializeAreas(patientsDir.filePath)
            }
        }
    }

    onSave: {
        var date = new Date
        patientsDir.currentFile = date.toISOString().replace(/[\:\.]/g, "-")

        if(provider.serialize(patientsDir.filePath)) {
            patientsDir.update()
        } else {
            patientsDir.currentFile = ""
        }
    }

    onClear: {
        deviceService.recording.reset()
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
}
