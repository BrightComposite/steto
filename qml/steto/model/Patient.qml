import QtQuick 2.0

import ru.applabs.storage 1.0

Record {
    property string surname
    property string name
    property string patronymic
    property int birth_year
    property int birth_month
    property int birth_day
    property string anamnesis

    readonly property string birthday: "%1.%2.%3".arg(birth_year).arg(('0' + birth_month).substr(-2)).arg(('0' + birth_day).substr(-2))

    function displayBirthday() {
        return "%1.%2.%3".arg(('0' + birth_day).substr(-2)).arg(('0' + birth_month).substr(-2)).arg(birth_year)
    }
}
