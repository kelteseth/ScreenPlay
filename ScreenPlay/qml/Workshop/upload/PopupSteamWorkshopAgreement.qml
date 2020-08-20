import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import ScreenPlay.Workshop 1.0 as SP
import ScreenPlay 1.0

Popup {
    id: popupSteamWorkshopAgreement
    dim: true
    width: 1100
    height: 600
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: Overlay.overlay
    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Material.background
    }

    Button {
        id:btnAbort
        text: qsTr("Abort Upload.")
        onClicked: {

        }
    }

    Button {
        id:btnAgree
        text: qsTr("I Agree to the Steam Workshop Agreement")
        highlighted: true
        Material.background: Material.accent
        Material.foreground: "white"
        onClicked: {

        }
    }
}
