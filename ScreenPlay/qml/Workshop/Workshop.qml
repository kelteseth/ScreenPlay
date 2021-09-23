import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Settings 1.0
import ScreenPlay 1.0

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
