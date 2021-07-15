import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import Qt5Compat.GraphicalEffects
import ScreenPlay 1.0
import "../Common"

Item {
    id: navigationWallpaperConfiguration

    width: 450
    states: []
    transitions: []

    anchors {
        top: parent.top
        right: parent.right
        bottom: parent.bottom
    }

    RippleEffect {
        id: rippleEffect

        target: navigationWallpaperConfiguration
    }

    Connections {
        function onActiveWallpaperCounterChanged() {
            rippleEffect.trigger();
        }

        target: ScreenPlay.screenPlayManager
    }

    Image {
        id: image

        width: 24
        height: 24
        source: "qrc:/assets/icons/icon_monitor.svg"

        anchors {
            rightMargin: 30
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        Text {
            id: txtAmountActiveWallpapers

            text: ScreenPlay.screenPlayManager.activeWallpaperCounter + ScreenPlay.screenPlayManager.activeWidgetsCounter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Material.accent
            font.pointSize: 10
            font.bold: true
            font.family: ScreenPlay.settings.font

            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 1
            }

        }

    }

    Text {
        id: activeMonitorName

        horizontalAlignment: Text.AlignRight
        color: Material.foreground
        font.pointSize: 12
        font.family: ScreenPlay.settings.font
        text: {
            if (ScreenPlay.screenPlayManager.activeWallpaperCounter > 0)
                return qsTr("Configurate active Wallpaper or Widgets");
            else
                return qsTr("No active Wallpaper or Widgets");
        }

        anchors {
            right: image.right
            rightMargin: 30
            verticalCenter: parent.verticalCenter
        }

    }

    MouseArea {
        id: ma

        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            ScreenPlay.util.setToggleWallpaperConfiguration();
        }
    }

}
