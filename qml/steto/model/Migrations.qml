import QtQuick 2.0

import ru.applabs.storage 1.0

Migrations {
    readonly property var initial: ({
        "patients": {
            primary: "id",
            columns: {
                id:         "INTEGER",
                name:       "VARCHAR(64)",
                surname:    "VARCHAR(64)",
                patronymic: "VARCHAR(64)",
                birthday:   "VARCHAR(10)",
                anamnesis:  "TEXT"
            }
        }
    })

    items: {
        "*": {
            to: "1.0",
            transaction: function(tx) {
                migration(tx, initial)
            }
        },
        "1.0": {
            to: "*",
            transaction: function(tx) {
                drop(tx, "*")
            }
        }
    }
}
