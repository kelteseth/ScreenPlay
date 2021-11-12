import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import Workshop 1.0
import ScreenPlay 1.0

Popup {
    id: popupOffline

    width: 1100
    height: 600
    modal: true
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: Overlay.overlay
    dim: true

    property ScreenPlayWorkshop workshop
    property SteamWorkshop steam

    Text {
        id: txtOffline

        anchors.centerIn: parent
        font.family: ScreenPlay.settings.font
        font.pointSize: 21
        color: Material.foreground
        text: qsTr("You need to run Steam for this. steamErrorRestart: %1 - steamErrorAPIInit: %2").arg(steam.steamErrorRestart).arg(steam.steamErrorAPIInit)
    }

    Button {
        highlighted: true
        text: qsTr("Back")
        onClicked: {
            ScreenPlay.util.setNavigation("Installed");
            popupOffline.close();
        }

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: txtOffline.bottom
        }

    }

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Material.background
    }

}
