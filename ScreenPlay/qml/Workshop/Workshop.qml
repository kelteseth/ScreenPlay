import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Settings
import ScreenPlay
import ScreenPlayWorkshop 1.0

Item {
    id: root

    property Item modalSource

    Component.onCompleted: {
        if (ScreenPlay.settings.steamVersion) {
            workshopLoader.setSource(
                        "qrc:/ScreenPlayWorkshop/qml/SteamWorkshop.qml", {
                            "modalSource": modalSource
                        })
        } else {
            workshopLoader.setSource("qrc:/ScreenPlayWorkshop/qml/Forum.qml")
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
