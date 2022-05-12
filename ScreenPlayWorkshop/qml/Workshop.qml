import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Settings
import ScreenPlay

Item {
    id: root

    property Item modalSource

    Component.onCompleted: {
        if (App.settings.steamVersion) {
            workshopLoader.setSource(
                        "qrc:/qml/ScreenPlayApp/qml/Workshop/SteamWorkshop.qml", {
                            "modalSource": modalSource
                        })
        } else {
            workshopLoader.setSource("qrc:/qml/ScreenPlayApp/qml/Workshop/Forum.qml")
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
