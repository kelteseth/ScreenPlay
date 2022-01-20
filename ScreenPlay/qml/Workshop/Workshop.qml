import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Settings
import ScreenPlay

Item {
    id: root

    required property Item modalSource

    Component.onCompleted: {
        if (ScreenPlay.settings.steamVersion) {
            workshopLoader.setSource(
                        "qrc:/ScreenPlay/qml/Workshop/SteamWorkshop.qml", {
                            "modalSource": modalSource
                        })
        } else {
            workshopLoader.setSource("qrc:/ScreenPlay/qml/Workshop/Forum.qml")
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
