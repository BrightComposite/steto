import QtQuick 2.0

import steto.view 1.0

MeasurementForm {
    id: page
    currentFile: wavDir.currentFile

    Component.onCompleted: {
        if(wavDir.fileExists) {
            provider.readWav(wavDir.filePath)
        } else {
            console.log("File", wavDir.filePath, "doesn't exist!")
        }
    }
}
