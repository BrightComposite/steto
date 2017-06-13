import QtQuick 2.7
import QtQuick.Controls 2.1

import ru.applabs.storage 1.0
import ru.applabs.lodash 1.0

import steto 1.0
import steto.view 1.0
import steto.model 1.0

Window {
    id: window

    title: "Стетоскоп"

    property var _: Lodash._

    property Component devicesView:             Component { Devices {} }
    property Component measurementView:         Component { Measurement {} }
    property Component analysisView:            Component { Analysis {} }
    property Component externalAnalysisView:    Component { ExternalAnalysis {} }
    property Component patientInfoView:         Component { PatientInfo {} }
    property Component patientsView:            Component { PatientsList {} }
    property Component createPatientView:       Component { CreatePatient {} }
    property Component editPatientView:         Component { EditPatient {} }
    property Component externalFilesView:       Component { ExternalFiles {} }
    property Component testAnalysisView:        Component { TestAnalysis {} }

    Storage {
        id: storage
        migrations: Migrations {}

        database: "stetoscope"
        version: "1.0"
    }

    DeviceService {
        id: deviceService
    }

    DirectoryModel {
        id: patientsDir
        currentPath: "patients/" + (patients.currentPatient ? patients.currentPatient.id : "default")
        extension: "txt"
        nameOnly: true
    }

    DirectoryModel {
        id: wavDir
        currentPath: "wav"
        extension: "wav"
    }

    PatientsRepo {
        id: patients
        storage: storage
    }

    Component.onCompleted: {
        storage.load()
    }
}
