import QtQuick 2.0

import ru.applabs 1.0

QtObject {
    id: migrations

    property var items: ({})

    function migrate(db, version) {
        db.transaction(function(tx) {
            var from = db.version

            do {
                var target = items[from !== "" ? from : "*"]

                if(!target) {
                    throw "Can't migrate database from v%1 to v%2!".arg(db.version).arg(version)
                }

                target.transaction(tx)
                from = target.to
            } while(target.to != version)
        })

        db.changeVersion(db.version, version, function(tx) {})

        return true
    }

    function migration(tx, tables) {
        for(var table in tables) {
            var data = tables[table],
                exists = tx.executeSql("SELECT name FROM sqlite_master WHERE type='table' AND name='%1'".arg(table)).rows.length > 0,
                scheme = {}

            data.primary = data.primary || "id"

            if(!exists) {
                create(tx, table, data)
                continue
            }

            data.add    = data.add    || {}
            data.rename = data.rename || {}
            data.modify = data.modify || {}
            data.remove = data.remove || []

            var rows = tx.executeSql("PRAGMA table_info(%1)".arg(table)).rows

            for(var i = 0; i < rows.length; ++i) {
                var row = rows.item(i)

                if(row.pk == 1) {
                    data.primary = row.name
                }

                scheme[row.name] = data.modify.hasOwnProperty(row.name) ? data.modify[row.name] : storage.info(row, true)
            }

            var fields = _.omit(_.assign(_.clone(scheme), data.add), data.remove)

            for(var f in data.rename) {
                var t = data.rename[f]
                fields[t] = fields[f]
                delete fields[f]
            }

            var s = [], from = []

            for(var field in fields) {
                if(fields[field] !== undefined) {
                    if(field == data.primary) {
                        fields[field] += " PRIMARY KEY"
                    }

                    s.push("%1 %2".arg(field).arg(fields[field]))

                    if(scheme.hasOwnProperty(field)) {
                        from.push(field)
                    }
                }
            }

            var to = from.slice()

            for(var key in data.rename) {
                to.push(data.rename[key])
                s.push("%1 %2".arg(key).arg(fields[key]))
                from.push(key)
            }

            console.log(JSON.stringify(from))
            console.log(JSON.stringify(to))

            if(tx.executeSql("SELECT name FROM sqlite_master WHERE type='table' AND name='tmp_%1'".arg(table)).rows.length > 0) {
                tx.executeSql("DROP TABLE tmp_%1".arg(table))
            }

            tx.executeSql("ALTER TABLE %1 RENAME TO tmp_%1".arg(table))
            tx.executeSql("CREATE TABLE %1 (%2) ".arg(table).arg(s.join(",")))
            tx.executeSql("INSERT INTO %1 (%2) SELECT %3 FROM tmp_%1".arg(table).arg(to.join(",")).arg(from.join(",")))
            tx.executeSql("DROP TABLE tmp_%1".arg(table))
        }
    }

    function create(tx, table, data) {
        console.log("create table '%1'".arg(table))

        var s = []

        for(var field in data.columns) {
            if(data.columns[field] !== undefined) {
                if(field == data.primary) {
                    data.columns[field] += " PRIMARY KEY"
                }

                s.push("%1 %2".arg(field).arg(data.columns[field]))
            }
        }

        tx.executeSql("CREATE TABLE %1 (%2) ".arg(table).arg(s.join(",")))
    }

    function drop(tx, tables) {
        if(Array.isArray(tables)) {
            for(var i = 0; i < tables.length; ++i) {
                drop(tx, tables[i])
            }

            return
        }

        if (tables == "*") {
            var rows = tx.executeSql("SELECT name FROM sqlite_master").rows

            for(var i = 0; i < rows.length; ++i) {
                drop(tx, rows.item(i).name)
            }

            return
        }

        if(tx.executeSql("SELECT name FROM sqlite_master WHERE type='table' AND name='%1'".arg(tables)).rows.length > 0) {
            tx.executeSql("DROP table '%1'".arg(tables))
        }
    }
}
