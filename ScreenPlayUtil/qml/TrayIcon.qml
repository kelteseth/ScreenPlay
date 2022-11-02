import QtQuick
import QtQuick.Window
import Qt.labs.platform
import ScreenPlayApp
import ScreenPlay

SystemTrayIcon {
    id: root

    property Window window
    visible: true
    icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/app.ico"
    tooltip: qsTr("ScreenPlay - Double click to change you settings.")
    onActivated: function (reason) {
        switch (reason) {
        case SystemTrayIcon.Unknown:
            break
        case SystemTrayIcon.Context:
            break
        case SystemTrayIcon.DoubleClick:
            window.show()
            break
        case SystemTrayIcon.Trigger:
            break
        case SystemTrayIcon.MiddleClick:
            break
        }
    }

    menu: Menu {
        MenuItem {
            text: qsTr("Open ScreenPlay")
            onTriggered: {
                window.show()
            }
        }

        MenuItem {
            id: miMuteAll

            property bool isMuted: true

            text: qsTr("Mute all")
            onTriggered: {
                if (miMuteAll.isMuted) {
                    isMuted = false
                    miMuteAll.text = qsTr("Mute all")
                    App.screenPlayManager.setAllWallpaperValue("muted",
                                                                      "true")
                } else {
                    isMuted = true
                    miMuteAll.text = qsTr("Unmute all")
                    App.screenPlayManager.setAllWallpaperValue("muted",
                                                                      "false")
                }
            }
        }

        MenuItem {
            id: miStopAll

            property bool isPlaying: false

            text: qsTr("Pause all")
            onTriggered: {
                if (miStopAll.isPlaying) {
                    isPlaying = false
                    miStopAll.text = qsTr("Pause all")
                    App.screenPlayManager.setAllWallpaperValue(
                                "isPlaying", "true")
                } else {
                    isPlaying = true
                    miStopAll.text = qsTr("Play all")
                    App.screenPlayManager.setAllWallpaperValue(
                                "isPlaying", "false")
                }
            }
        }

        MenuItem {
            text: qsTr("Quit ScreenPlay")
            onTriggered: App.exit()
        }
    }
}
