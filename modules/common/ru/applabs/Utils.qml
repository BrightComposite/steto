pragma Singleton

import QtQuick 2.0

QtObject {
    id: utils

    function delay(interval, cb) {
        function Timer() {
            return Qt.createQmlObject("import QtQuick 2.0; Timer {}", utils)
        }

        var timer = new Timer
        timer.interval = interval
        timer.repeat = false

        if(cb) {
            timer.triggered.connect(cb)
        }

        timer.start()

        return timer
    }

    function date() {
        return new Date()
    }

    function timestamp() {
        return Math.floor(new Date().getTime() / 1000)
    }

    function daysInMonth(month, year) {
        return new Date(year, month, 0).getDate();
    }

    function assign(obj, key, val) {
        if(val === null || val === undefined) {
            if(typeof obj[key] == "string") {
                obj[key] = ""
            } else {
                obj[key] = 0
            }
        } else {
            obj[key] = val
        }
    }
}
