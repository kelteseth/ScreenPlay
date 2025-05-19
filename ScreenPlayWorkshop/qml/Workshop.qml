import QtQuick

Item {
    id: root

    property Item modalSource

    Component.onCompleted: {
        if (App.globalVariables.isSteamVersion()) {
            workshopLoader.setSource("qml/Workshop/SteamWorkshop.qml", {
                "modalSource": modalSource
            });
        } else {
            workshopLoader.setSource("qml/Workshop/Forum.qml");
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
