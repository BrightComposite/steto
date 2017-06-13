import QtQuick 2.0

ExternalFilesForm {
    title: "Внешние файлы"

    onSelectFile: {
        wavDir.currentFile = filename
        viewport.push(externalAnalysisView)
    }
}
