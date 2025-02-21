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
            break;
        case SystemTrayIcon.Context:
            break;
        case SystemTrayIcon.DoubleClick:
            applicationWindow.show();
            break;
        case SystemTrayIcon.Trigger:
            break;
        case SystemTrayIcon.MiddleClick:
            break;
        }
    }

    function open() {
        App.showDockIcon(true);
        applicationWindow.show();
    }

    menu: Menu {
        MenuItem {
            id: miOpenScreenPlay
            text: qsTr("Open ScreenPlay")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_open_in_new_black.svg"
            onTriggered: {
                root.open();
            }
        }
        MenuItem {
            id: miChangeWallpaperSettings
            text: qsTr("Change Wallpaper settings")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_video_settings.svg"
            onTriggered: {
                root.open();
                App.uiAppStateSignals.setNavigation("Installed");
                App.uiAppStateSignals.setToggleWallpaperConfiguration();
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
                root.open();
                App.uiAppStateSignals.setNavigation("Workshop");
            }
        }

        MenuItem {
            id: miCreate
            text: qsTr("Create new Wallpaper or Widgets")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_plus.svg"
            onTriggered: {
                root.open();
                App.uiAppStateSignals.setNavigation("Create");
            }
        }
        MenuItem {
            id: miSettings
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_settings.svg"
            text: qsTr("Settings")
            onTriggered: {
                root.open();
                App.uiAppStateSignals.setNavigation("Settings");
            }
        }

        MenuItem {
            separator: true
        }
        MenuItem {
            text: qsTr("Forums and Help")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_supervisor_account.svg"
            onTriggered: {
                Qt.openUrlExternally("https://forum.screen-play.app/");
            }
        }
        MenuItem {
            text: qsTr("Frequently Asked Questions (FAQ)")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_help_center.svg"
            onTriggered: {
                Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/Frequently%20Asked%20Questions/");
            }
        }
        MenuItem {
            separator: true
        }
        MenuItem {
            id: miMuteAll

            property bool isMuted: false

            text: qsTr("Mute all")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_volume_mute.svg"
            onTriggered: {
                if (miMuteAll.isMuted) {
                    isMuted = false;
                    miMuteAll.text = qsTr("Mute all");
                    miMuteAll.icon.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_volume_mute.svg";
                    App.screenPlayManager.setAllWallpaperValue("muted", "true");
                } else {
                    isMuted = true;
                    miMuteAll.text = qsTr("Unmute all");
                    miMuteAll.icon.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_volume_up.svg";
                    App.screenPlayManager.setAllWallpaperValue("muted", "false");
                }
            }
        }

        MenuItem {
            id: miStopAll

            property bool isPlaying: false

            text: qsTr("Pause all")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_pause.svg"
            onTriggered: {
                if (miStopAll.isPlaying) {
                    isPlaying = false;
                    miStopAll.text = qsTr("Pause all");
                    miStopAll.icon.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_pause.svg";
                    App.screenPlayManager.setAllWallpaperValue("isPlaying", "true");
                } else {
                    isPlaying = true;
                    miStopAll.text = qsTr("Play all");
                    miStopAll.icon.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_play.svg";
                    App.screenPlayManager.setAllWallpaperValue("isPlaying", "false");
                }
            }
        }

        MenuItem {
            text: qsTr("Quit ScreenPlay")
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_cancel_presentation.svg"
            onTriggered: App.exit()
        }
    }
}
