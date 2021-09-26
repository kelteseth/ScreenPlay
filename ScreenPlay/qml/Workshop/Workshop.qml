import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts 
import Settings
import ScreenPlay

Item {
    id: root

    Component.onCompleted: {
        if (ScreenPlay.settings.steamVersion) {
            workshopLoader.setSource("qrc:/qml/Workshop/SteamWorkshop.qml")
        } else {
            workshopLoader.setSource("qrc:/qml/Workshop/Forum.qml")
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
