import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import ScreenPlayApp

Item {
    id: root

    property Item modalSource

    Component.onCompleted: {
        if (App.globalVariables.isSteamVersion()) {
            workshopLoader.setSource("qrc:/qml/ScreenPlayApp/qml/Workshop/SteamWorkshop.qml", {
                "modalSource": modalSource
            });
        } else {
            workshopLoader.setSource("qrc:/qml/ScreenPlayApp/qml/Workshop/Forum.qml");
        }
    }

    Loader {
        id: workshopLoader
        asynchronous: true
        anchors.fill: parent
    }
}
