import QtQuick 2.0

import ru.applabs 1.0

QtObject {
    id: model

    property Storage storage

    property string table: ""
    property var scheme: ({})
    readonly property var fields: Object.keys(scheme)
    property string primaryKey: "id"

    property Component recordScheme: null
    property Record example: recordScheme ? recordScheme.createObject(model) : null

    property var transform: ({})
    property var map: ({})
    property var unmap: ({})

    property var readonly: []

    property var mappings: _.assign(_.zipObject(keys.common, keys.common), transform)

    signal loaded
    signal creating(Record record)
    signal deleting(string key, var value)

    property QtObject keys: QtObject {
        id: keys

        property var scheme: _.keys(model.scheme)
        property var record: model.example ? _.keys(model.example).filter(function(key) { return !(key == "objectName" || key.indexOf("Changed") !== -1 || typeof model.example[key] == "function" || readonly.indexOf(key) !== -1) }) : keys.scheme
        property var map: _.keys(model.map)
        property var unmap: _.keys(model.unmap)
        property var common: _.intersection(keys.scheme, keys.record)
        property var all: _.keys(model.mappings)
        property var nomap: _.difference(keys.all, keys.map)
        property var nounmap: _.difference(keys.all, keys.unmap)
    }

    property Component builder: QtObject {
        property var opt: ({})

        function options(o) {
            opt = _.assign(opt, o)

            if(opt.orderBy) {
                if(!Array.isArray(opt.orderBy)) {
                    opt.orderBy = [opt.orderBy, "ASC"]
                }
            }

            return this
        }

        function get(key, value) {
            return query(function(b) {
                if(opt.orderBy) {
                    b.orderBy(opt.orderBy)
                }

                if(opt.limit) {
                    b.limit(opt.limit)
                }

                if(opt.offset) {
                    b.offset(opt.offset)
                }

                if(!key) {
                    return unserialize(b.select())
                }

                if(Array.isArray(key) || typeof key == "object") {
                    return unserialize(b.where(key).select())
                }

                if(!value) {
                    value = key
                    key = primaryKey
                }

                return unserialize(b.where(key, value).select())
            })
        }
    }

    /**
     *  Creates query builder, apply options and return it
     */
    function options(opt) {
        return builder.createObject(model).options(opt)
    }

    /**
     *  Get records from the model
     *  If the second parameter is not specified, then the value will be the
     *  first parameter and the key will be the primaryKey
     *  If even the first parameter is not specified, then return all records
     */
    function get(key, value) {
        return builder.createObject(model).get(key, value)
    }

    /**
     *  Get first record from the model
     *  If the second parameter is not specified, then the value will be the
     *  first parameter and the key will be the primaryKey
     *  If even the first parameter is not specified, then return first record in the table
     */
    function first(key, value) {
        var records = get(key, value, 1)
        return records.length > 0 ? records[0] : null
    }

    /**
     *  Check whether record exists
     */
    function has(key, value) {
        return query(function(b) {
            b.limit(1)

            if(!key) {
                return b.select().length > 0
            }

            if(Array.isArray(key) || typeof key == "object") {
                return b.where(key).select().length > 0
            }

            if(!value) {
                value = key
                key = primaryKey
            }

            return b.where(key, value).select().length > 0
        })
    }

    /**
     *  Add a record to the model
     *  If the second parameter is not specified, then the record will be the
     *  first parameter and the key will be the primaryKey
     */
    function add(key, records) {
        if(!records) {
            records = key
            key = primaryKey
        }

        if(!Array.isArray(records)) {
            records = [records]
        }

        query(function(b) {
            for(var i = 0; i < records.length; ++i)  {
                var record = records[i]
                creating(record)
                b.insert(serialize(record))
            }
        })
    }

    /**
     *  Save records or a single record
     *  If the fields variable is specified, save only these fields
     */
    function save(r, fields) {
        if(!Array.isArray(r)) {
            r = [r]
        }

        for(var i = 0; i < r.length; ++i) {
            if(fields) {
                query(function(b) { b.update(primaryKey, serialize(r[i], _.union(fields, [primaryKey]))) })
            } else {
                query(function(b) { b.save(primaryKey, serialize(r[i])) })
            }
        }
    }

    /**
     *  Remove records from the model by values
     *  If the second parameter is not specified, then the values will be the
     *  first parameter and the key will be the primaryKey
     */
    function remove(key, values) {
        if(!values) {
            values = key
            key = primaryKey
        }

        if(!Array.isArray(values)) {
            values = [values]
        }

        query(function(b) {
            for(var i = 0; i < values.length; ++i) {
                deleting(key, values[i])
                b.where(key, values[i]).remove()
            }
        })
    }

    /**
     *  Remove records from the model
     *  If the second parameter is not specified, then the record will be the
     *  first parameter and the key will be the primaryKey
     */
    function removeRecords(key, records) {
        if(!records) {
            records = key
            key = primaryKey
        }

        if(!Array.isArray(records)) {
            records = [records]
        }

        query(function(b) {
            for(var i = 0; i < records.length; ++i) {
                deleting(key, records[i][key])
                b.where(key, records[i][key]).remove()
            }
        })
    }

    /**
     *  Clear the model
     */
    function clear() {
        query(function(b) { b.clear() })
    }

    /**
     *  Transforms record to the table row format
     */
    function serialize(record, fields) {
        return example ? record.save(fields) : fields ? _.pick(record, fields) : record
    }

    /**
     *  Transforms the raw table data to records or a single record
     */
    function unserialize(rows) {
        if(rows === null || rows === undefined) {
            return null
        }

        if(typeof rows.item == "function") {
            var r = []

            for(var i = 0; i < rows.length; ++i) {
                r.push(example ? unserialize(rows.item(i)) : rows.item(i))
            }

            return r
        }

        return example ? recordScheme.createObject(model).load(rows) : rows
    }

    function query(cb) {
        return storage.builder(table, cb)
    }

    property Connections connections: Connections {
        target: storage

        onLoaded: {
            console.log("Table:", table)
            var rows

            storage.transaction(function(tx) {
                rows = tx.executeSql("PRAGMA table_info(%1)".arg(table)).rows
            })

            var fields = {}

            for(var i = 0; i < rows.length; ++i) {
                var field = rows.item(i)
                var info = storage.info(field)
                fields[field.name] = info

                if(info.indexOf("PRIMARY KEY") !== -1) {
                    model.primaryKey = field.name
                }

                console.log("    %1: %2".arg(field.name).arg(info))
            }

            console.log("  * primary key:", model.primaryKey)

            scheme = fields
            loaded()
        }
    }
}
