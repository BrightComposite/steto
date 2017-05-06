import QtQuick 2.5

import ru.applabs 1.0
import ru.applabs.tasks 1.0
import ru.applabs.io.private 1.0

Http {
    id: http

    property string token: ""

    property bool hasConnection: true

    function task(data) {
        if (token) {
            data.headers = data.headers || {}
            data.headers["Accept"] = "application/json";
            data.headers["Authorization"] = "Bearer " + token;
        }

        return Scheduler.task(function(t) {
            Scheduler.task(http.send, data)
                .success(function(subtask) {
                    console.log("Response:", subtask.data.response)
                    t.write(subtask.data).succeed()
                }).fail(function(subtask) {
                    subtask.write({message: internetController.whatIsWrong(subtask.data.status, subtask.data.response)})
                    t.write(subtask.data).fail()
                }).run()
        })
    }
}
