import QtQuick 2.9

Item {
    id: workshopLoader

    property bool available: true

    Loader {
        id: wsLoader
        source: "qrc:/qml/Workshop/WorkshopAvailableTest.qml"
        onStatusChanged: {
            if (status == Loader.Ready) {
                available = true
            } else if (status == Loader.Error) {
                available = false
            }
            print(available)
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
