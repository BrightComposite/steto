import QtQuick 2.0

DevicesForm {
    id: page

    model: deviceService.devices

    onSearch: {
        if(deviceService.isValid) {
            deviceService.search()
        }
    }

    onSelect: {
        deviceService.select(address)
    }
}
