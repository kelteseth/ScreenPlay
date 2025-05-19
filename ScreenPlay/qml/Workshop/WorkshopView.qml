import QtQuick

Item {
    id: root

    property Item modalSource

    Component.onCompleted: {
        if (App.globalVariables.isSteamVersion()) {
            workshopLoader.setSource("qrc:/qt/qml/ScreenPlayWorkshop/qml/SteamWorkshop.qml", {
                "modalSource": modalSource
            });
        } else {
            workshopLoader.setSource("qrc:/qt/qml/ScreenPlayWorkshop/qml/Forum.qml");
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
