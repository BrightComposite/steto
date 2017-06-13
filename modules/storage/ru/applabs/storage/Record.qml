import QtQuick 2.0

import ru.applabs 1.0

QtObject {
    id: record

    property int id: 0

    signal changed

    function set(key, val) {
        if(record.hasOwnProperty(key)) {
            record[key] = val
            changed()
        }
    }

    function get(key) {
        return record.hasOwnProperty(key) ? record[key] : null
    }

    /**
     *  Overwrite values in record
     */
    function write(obj) {
        var keys = Object.keys(obj).filter(function(key) {
            return model.keys.record.indexOf(key) !== -1
        })

        for(var i = 0; i < keys.length; ++i) {
            var key = keys[i]

            if(record.hasOwnProperty(key)) {
                Utils.assign(record, key, obj[key])
            }
        }

        changed()
        return this
    }

    /**
     *  Load from the table row
     *
     *  @params:
     *      object row
     *      [array fields]
     */
    function load(row, fields) {
        var keys = model.keys.nomap,
            mapKeys = model.keys.map

        if(fields) {
            keys = _.difference(keys, fields)
            mapKeys = _.difference(mapKeys, fields)
        }

        for(var i = 0; i < keys.length; ++i) {
            var key = keys[i]
            Utils.assign(record, key, row[model.mappings[key]])
        }

        for(var i = 0; i < mapKeys.length; ++i) {
            model.map[mapKeys[i]](row, record)
        }

        changed()
        return this
    }

    /**
     *  Save to the table row
     *
     *  @params:
     *      [array fields] - row fields
     */
    function save(fields) {
        var row = {},
            keys = model.keys.nounmap,
            unmapKeys = model.keys.unmap

        if(fields) {
            keys = _.difference(keys, fields)
            unmapKeys = _.difference(unmapKeys, fields)
        }

        for(var i = 0; i < keys.length; ++i) {
            var key = keys[i]
            row[model.mappings[key]] = record[key]
        }

        for(var i = 0; i < unmapKeys.length; ++i) {
            model.unmap[unmapKeys[i]](row, record)
        }

        return row
    }
}
