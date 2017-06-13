import QtQuick 2.0

import ru.applabs.storage 1.0

Repository {
    id: repo

    model: Model {
        storage: repo.storage

        table: "patients"
        recordScheme: Patient {}

        map: {
            "birthday": function(row, record) {
                var data = row.birthday.split(".")
                record.birth_year = parseInt(data[0])
                record.birth_month = parseInt(data[1])
                record.birth_day = parseInt(data[2])
            }
        }
    }

    property Patient currentPatient: null

    property Storage storage

    function load() {
        records = model.options({
            orderBy: "surname"
        }).get()
    }
}
