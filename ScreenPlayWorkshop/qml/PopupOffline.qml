import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import ScreenPlayWorkshop
import ScreenPlayUtil
import ScreenPlay
import ScreenPlayApp

Popup {
    id: root

    width: 1100
    height: 600
    modal: true
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: Overlay.overlay
    dim: true

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Material.background
    }
    required property ScreenPlayWorkshop workshop
    required property SteamWorkshop steam
    //required property Item modalSource
    Overlay.modal: ModalBackgroundBlur {
        sourceItem: root.modalSource
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 40
        spacing: 20
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Image {
            source: "qrc:/qml/ScreenPlayWorkshop/assets/images/steam_offline.png"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            font.pointSize: 16
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
            color: Material.primaryTextColor
            text: qsTr("You need to run Steam to access the Steam Workshop")
        }
        Text {
            font.pointSize: 12
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
            color: Material.secondaryTextColor
            text: qsTr("Steam Error Restart: %1\nSteam Error API Init: %2").arg(steam.steamErrorRestart).arg(root.steam.steamErrorAPIInit)
        }

        Button {
            highlighted: true
            text: qsTr("Back")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                root.close();
                App.util.setNavigation("Installed");
            }
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
