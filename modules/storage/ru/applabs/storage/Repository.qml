import QtQuick 2.0

QtObject {
    property Model model
    property var records: []

    property int size: records ? records.length : 0

    property Connections connections: Connections {
        target: model
        onLoaded: load()
    }

    function get(i) {
        return records[i]
    }

    function set(i, r) {
        records[i] = r
    }

    /**
     *  Find a first record by the predicate.
     *  If the second parameter is not specified, then the value will be the
     *  first parameter and the key will be the primaryKey
     *
     *  a - key or predicate
     *  b - optional value
     */
    function find(a, b) {
        if(typeof a == "function" || typeof a == "object") {
            return _.find(records, a)
        }

        return _.find(records, typeof b != "undefined" ? [a, b] : [model.primaryKey, a])
    }

    /**
     *  Find all records by the predicate.
     *  If the second parameter is not specified, then the value will be the
     *  first parameter and the key will be the primaryKey
     *
     *  a - key or predicate
     *  b - optional value
     */
    function filter(a, b) {
        if(typeof a == "function" || typeof a == "object") {
            return _.filter(records, a)
        }

        return _.filter(records, typeof b != "undefined" ? [a, b] : [model.primaryKey, a])
    }

    /**
     *  Find an index of the record
     *  If the second parameter is not specified, then the record will be the
     *  first parameter and the key will be the primaryKey
     */
    function indexOf(key, record) {
        if(!record) {
            record = key
            key = model.primaryKey
        }

        for(var i = 0; i < records.length; ++i) {
            var r = records[i]

            if(r[key] == record[key])
                return i
        }

        return -1
    }

    /**
     *  May be overloaded to initialize the repo in a custom way
     */
    function load() {
        records = model.get()
    }

    /**
     *  May be overloaded to uninitialize the repo in a custom way
     */
    function unload() {
        records = []
    }

    function reload() {
        unload()
        load()
    }
}
