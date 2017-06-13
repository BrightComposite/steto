import QtQuick 2.0

EditPatientForm {
    title: "Редактирование данных"

    completeText: "Сохранить"
    initial: patients.currentPatient

    onComplete: {
        var id = patients.currentPatient.id

        patients.model.save(patients.model.unserialize({
            id: id,
            surname: data.surname,
            name: data.name,
            patronymic: data.patronymic,
            birthday: data.birthday,
            anamnesis: data.anamnesis,
        }))

        patients.reload()
        patients.currentPatient = patients.find(id)

        viewport.pop()
    }
}
