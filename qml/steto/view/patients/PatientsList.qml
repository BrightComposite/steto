import QtQuick 2.0

PatientsListForm {
    title: "Пациенты"

    onAddPatient: {
        viewport.push(createPatientView)
    }

    onSelectPatient: {
        patients.currentPatient = patient
        viewport.push(patientInfoView)
    }
}
