import QtQuick 2.0

VisualizationForm {
    onToggle: {
        deviceService.data.toggle()
    }
}
