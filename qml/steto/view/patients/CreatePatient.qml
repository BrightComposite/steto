import QtQuick 2.0

EditPatientForm {
    title: "Новый пациент"

    completeText: "Добавить"

    onComplete: {
        patients.model.add(patients.model.unserialize({
            surname: data.surname,
            name: data.name,
            patronymic: data.patronymic,
            birthday: data.birthday,
            anamnesis: data.anamnesis,
        }))

        patients.reload()
        viewport.pop()
    }
}
