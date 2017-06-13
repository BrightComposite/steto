import QtQuick 2.0
import QtQuick.Controls 2.1

import steto.view 1.0

PatientInfoForm {
    title: "Информация о пациенте"

    onEdit: {
        viewport.push(editPatientView)
    }

    onRemove: {
        removeDialog.open()
    }

    onNewMeasurement: {
        viewport.push(measurementView)
    }

    onSelectFile: {
        patientsDir.currentFile = filename
        viewport.push(analysisView)
    }

    Dialog {
        id: removeDialog

        text: "Удалить пациента?"

        onAccepted: {
            patients.model.removeRecords(patients.currentPatient)
            patients.currentPatient = null
            patients.reload()
            viewport.pop()
        }
    }

    StackView.onActivated: {
        patientsDir.currentFile = ""
    }
}
