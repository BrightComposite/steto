import QtQuick 2.0
import QtQuick.LocalStorage 2.0

QtObject {
    id: storage

    property var db: null
    property string database: ""
    property string version: "1.0"
    property int size: 1000000

    property Component builderScheme: QueryBuilder {}
    property Migrations migrations: Migrations {}

    signal loaded

    function transaction(cb) {
        var result

        db.transaction(function(tx) {
            result = cb(tx)
        })

        return result
    }

    function makeBuilder(table, tx) {
        return builderScheme.createObject(storage, {table: table, tx: tx})
    }

    function builder(table, cb) {
        return transaction(function(tx) {
            return cb(makeBuilder(table, tx))
        })
    }

    function load() {
        db = LocalStorage.openDatabaseSync(database, "", database + "local storage", 1000000)

        if(db.version != version) {
            console.log("Migrate database from v%1 to v%2...".arg(db.version).arg(version))

            if(!migrations.migrate(db, version)) {
                return
            }
        }

        loaded()
    }

    function info(field, ignore_primary) {
        var info = field.type

        if(field.pk == 1 && !ignore_primary) {
            info += " PRIMARY KEY"
        }

        if(field.notnull == 1) {
            info += " NOT NULL"
        }

        if(field.dflt_value !== null) {
            info += " DEFAULT " + field.dflt_value
        }

        return info
    }
}
