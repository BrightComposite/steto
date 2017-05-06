import QtQuick 2.0

DeviceListForm {
    id: page

    list {
        model: deviceService.devices
    }

    onSearch: {
        deviceService.search()
    }

    onSelect: {
        deviceService.select(address)
    }
}
