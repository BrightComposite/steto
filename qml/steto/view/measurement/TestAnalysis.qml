import QtQuick 2.0

import steto.test 1.0

MeasurementForm {
    id: page
    title: "Тест"

    TestSampleGenerator {
        id: generator
    }

    provider: generator.provider
}
