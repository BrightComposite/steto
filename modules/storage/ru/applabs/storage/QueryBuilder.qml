import QtQuick 2.0

import ru.applabs 1.0

QtObject {
    id: builder
    property string table: ""
    property var tx: null

    property QtObject d: QtObject {
        property var where: null
        property var orderBy: null
        property int offset: 0
        property int limit: 0
        property var params: ({})

        function makeWhere(where) {
            if(where.length < 2) {
                throw "Where clause must have 2 or more params"
            }

            if(typeof where[1] == "undefined") {
                throw "The second parameter of where clause may not be undefined"
            }

            if(where.length == 2 || typeof where[2] == "undefined") {
                if(Array.isArray(where[1])) {
                    var v = []

                    for(var i = 0; i < where[1].length; ++i) {
                        v.push(where[1][i])
                    }

                    return [v.map(function() { return "%1=?".arg(where[0]) }).join(" OR "), v]
                }

                return ["%1=?".arg(where[0]), [where[1]]]
            }

            return ["%1 %2 ?".arg(where[0]).arg(where[1]), [where[2]]]
        }

        function makeTail() {
            var tail = ""

            if(orderBy) {
                tail += " ORDER BY %1 %2".arg(orderBy[0]).arg(orderBy[1])
            }

            if(offset > 0) {
                tail += " OFFSET " + offset
            }

            if(limit > 0) {
                tail += " LIMIT " + limit
            }

            return tail
        }

        function clear() {
            where = null
            orderBy = null
            offset = 0
            limit = 0
            params = {}
        }
    }

    function add(obj, key) {
        if(key) {
            var x = {}
            x[key] = obj[key]
            obj = x
        }

        d.params = _.assign(d.params, obj)
        return builder
    }

    function orderBy(o) {
        d.orderBy = o
        return builder
    }

    function offset(o) {
        d.offset = o
        return builder
    }

    function limit(l) {
        d.limit = l
        return builder
    }

    function where(statement, a, b) {
        if(!statement) {
            d.where = null
            return builder
        }

        if(!Array.isArray(statement)) {
            if(typeof statement == "object") {
                return where(_.entries(statement))
            }

            d.where = d.makeWhere([statement, a, b])
            return builder
        }

        if(!Array.isArray(statement[0])) {
            d.where = d.makeWhere(statement)
            return builder
        }

        var params = []

        for(var i = 0; i < statement.length; ++i) {
            params.push(d.makeWhere(statement[i]))
        }

        var query = params.map(function(e) { return e[0] }).join(" AND ")
        var values = params.reduce(function(t, e) { return t.concat(e[1]) }, [])

        d.where = [query, values]
        return builder
    }

    function select() {
        var tail = d.makeTail()

        if(!d.where) {
            return tx.executeSql("SELECT * FROM %1 %2".arg(table).arg(tail)).rows
        }

        return tx.executeSql("SELECT * FROM %1 WHERE %2 %3".arg(table).arg(d.where[0]).arg(tail), d.where[1]).rows
    }

    function remove() {
        if(!d.where) {
            return tx.executeSql("DELETE FROM %1".arg(table))
        }

        return tx.executeSql("DELETE FROM %1 WHERE %2".arg(table).arg(d.where[0]), d.where[1])
    }

    function insert(obj) {
        if(obj) {
            add(obj);
        }

        var fields = _.without(_.keys(d.params), "id");

        if(fields.length > 0) {
            var query = fields.join(", "),
                values = fields.map(function(e) { return d.params[e] }),
                placeholders = values.map(function() { return "?" }).join(", ")

            tx.executeSql("INSERT INTO %1 (%2) VALUES (%3)".arg(table).arg(query).arg(placeholders), values)
        }
    }

    function update(where, obj) {
        if(obj) {
            add(obj)
        }

        var fields = _.keys(d.params)

        if(fields.length > 0) {
            if(!Array.isArray(where)) {
                if(obj) {
                    where = [where, obj[where]]
                } else {
                    throw "where must be an array ([key, value])"
                }
            }

            var values = fields.map(function(e) { return d.params[e] }),
                query = "UPDATE %1 SET %2=? WHERE %3=?"
                    .arg(this.table).arg(fields.join("=?, ")).arg(where[0])


            tx.executeSql(query, values.concat([where[1]]))
        }
    }

    function save(query, obj) {
        if(obj) {
            add(obj)
        }

        var fields = _.keys(d.params)

        if(fields.length > 0) {
            if(!Array.isArray(query)) {
                if(obj) {
                    query = [query, obj[query]]
                } else {
                    throw "query must be an array ([key, value])"
                }
            }

            var rows = where(query).select()

            if(rows.length > 0) {
                tx.executeSql("UPDATE %1 SET %2=? WHERE %3=?"
                    .arg(table).arg(fields.join("=?, ")).arg(query[0]), _.values(d.params).concat([query[1]]))
            } else {
                fields = _.without(fields, "id")

                var values = _.map(fields, function(e) { return d.params[e]; })
                var placeholders = values.map(function() { return "?" })

                tx.executeSql("INSERT INTO %1 (%2) VALUES (%3)"
                    .arg(table).arg(fields.join(", ")).arg(placeholders.join(", ")), values)
            }
        }
    }

    function clear() {
        return remove()
    }

    function reset() {
        d.clear()
        return builder
    }
}
