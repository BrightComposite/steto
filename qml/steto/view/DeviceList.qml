import QtQuick 2.0

DeviceListForm {
    id: page

    model: deviceService.devices

    onSearch: {
        deviceService.search()
    }

    onSelect: {
        deviceService.select(address)
    }
}
