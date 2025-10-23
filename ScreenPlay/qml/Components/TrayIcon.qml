import QtQuick
import QtQuick.Window
import Qt.labs.platform
import ScreenPlay

SystemTrayIcon {
    id: root

    property Window applicationWindow
    visible: true
    icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/app.ico"
    tooltip: qsTr("ScreenPlay - Double click to change you settings.")
    onActivated: function (reason) {
        switch (reason) {
        case SystemTrayIcon.Unknown:
            break
        case SystemTrayIcon.Context:
            break
        case SystemTrayIcon.DoubleClick:
            applicationWindow.show()
            break
        case SystemTrayIcon.Trigger:
            break
        case SystemTrayIcon.MiddleClick:
            break
        }
    }

    function open() {
        App.showDockIcon(true)
        applicationWindow.show()
    }

    menu: Menu {
        MenuItem {
            id: miOpenScreenPlay
            text: qsTr("Open ScreenPlay")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_open_in_new_black.svg"
            onTriggered: {
                root.open()
            }
        }
        MenuItem {
            id: miChangeWallpaperSettings
            text: qsTr("Change Wallpaper settings")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_video_settings.svg"
            onTriggered: {
                root.open()
                App.uiAppStateSignals.setNavigation("Installed")
                App.uiAppStateSignals.setToggleWallpaperConfiguration()
            }
        }
        MenuItem {
            separator: true
        }

        MenuItem {
            text: qsTr("Browse Workshop")
            enabled: App.globalVariables.isSteamVersion()
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_steam.svg"
            onTriggered: {
                root.open()
                App.uiAppStateSignals.setNavigation("Workshop")
            }
        }

        MenuItem {
            id: miCreate
            text: qsTr("Create new Wallpaper or Widgets")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_plus.svg"
            onTriggered: {
                root.open()
                App.uiAppStateSignals.setNavigation("Create")
            }
        }
        MenuItem {
            id: miSettings
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_settings.svg"
            text: qsTr("Settings")
            onTriggered: {
                root.open()
                App.uiAppStateSignals.setNavigation("Settings")
            }
        }

        MenuItem {
            separator: true
        }
        MenuItem {
            text: qsTr("Forums and Help")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_supervisor_account.svg"
            onTriggered: {
                Qt.openUrlExternally("https://forum.screen-play.app/")
            }
        }
        MenuItem {
            text: qsTr("Frequently Asked Questions (FAQ)")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_help_center.svg"
            onTriggered: {
                Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/frequently-asked-questions/")
            }
        }
        MenuItem {
            separator: true
        }
        MenuItem {
            id: miMuteAll

            text: App.screenPlayManager.isMuted ? qsTr("Unmute all") : qsTr("Mute all")
            icon.source: App.screenPlayManager.isMuted ? "qrc:/qt/qml/ScreenPlay/assets/icons/icon_volume_up.svg" : "qrc:/qt/qml/ScreenPlay/assets/icons/icon_volume_mute.svg"
            onTriggered: {
                App.screenPlayManager.isMuted = !App.screenPlayManager.isMuted
            }
        }

        MenuItem {
            id: miStopAll

            text: App.screenPlayManager.isPaused ? qsTr("Play all") : qsTr("Pause all")
            icon.source: App.screenPlayManager.isPaused ? "qrc:/qt/qml/ScreenPlay/assets/icons/icon_play.svg" : "qrc:/qt/qml/ScreenPlay/assets/icons/icon_pause.svg"
            onTriggered: {
                App.screenPlayManager.isPaused = !App.screenPlayManager.isPaused
            }
        }

        MenuItem {
            text: qsTr("Quit ScreenPlay")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_cancel_presentation.svg"
            onTriggered: App.exit()
        }
    }
}
