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
            break;
        case SystemTrayIcon.Context:
            break;
        case SystemTrayIcon.DoubleClick:
            window.show();
            break;
        case SystemTrayIcon.Trigger:
            break;
        case SystemTrayIcon.MiddleClick:
            break;
        }
    }

    function open() {
        App.showDockIcon(true);
        window.show();
    }

    menu: Menu {
        MenuItem {
            id: miOpenScreenPlay
            text: qsTr("Open ScreenPlay")
            onTriggered: {
                root.open();
            }
        }
        MenuItem {
            id: miChangeWallpaperSettings
            text: qsTr("Change Wallpaper settings")
            onTriggered: {
                root.open();
                App.util.setNavigation("Installed");
                App.util.setToggleWallpaperConfiguration();
            }
        }
        MenuItem {
            separator: true
        }

        MenuItem {
            text: qsTr("Browse Workshop")
            onTriggered: {
                root.open();
                App.util.setNavigation("Workshop");
            }
        }

        MenuItem {
            id: miCreate
            text: qsTr("Create new Wallpaper or Widgets")
            onTriggered: {
                root.open();
                App.util.setNavigation("Create");
            }
        }
        MenuItem {
            id: miSettings
            text: qsTr("Settings")
            onTriggered: {
                root.open();
                App.util.setNavigation("Settings");
            }
        }

        MenuItem {
            separator: true
        }
        MenuItem {
            text: qsTr("Forums")
            onTriggered: {
                Qt.openUrlExternally("https://forum.screen-play.app/");
            }
        }
        MenuItem {
            text: qsTr("Frequently Asked Questions (FAQ)")
            onTriggered: {
                Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/Frequently%20Asked%20Questions/");
            }
        }
        MenuItem {
            separator: true
        }
        MenuItem {
            id: miMuteAll

            property bool isMuted: true

            text: qsTr("Mute all")
            onTriggered: {
                if (miMuteAll.isMuted) {
                    isMuted = false;
                    miMuteAll.text = qsTr("Mute all");
                    App.screenPlayManager.setAllWallpaperValue("muted", "true");
                } else {
                    isMuted = true;
                    miMuteAll.text = qsTr("Unmute all");
                    App.screenPlayManager.setAllWallpaperValue("muted", "false");
                }
            }
        }

        MenuItem {
            id: miStopAll

            property bool isPlaying: false

            text: qsTr("Pause all")
            onTriggered: {
                if (miStopAll.isPlaying) {
                    isPlaying = false;
                    miStopAll.text = qsTr("Pause all");
                    App.screenPlayManager.setAllWallpaperValue("isPlaying", "true");
                } else {
                    isPlaying = true;
                    miStopAll.text = qsTr("Play all");
                    App.screenPlayManager.setAllWallpaperValue("isPlaying", "false");
                }
            }
        }

        MenuItem {
            text: qsTr("Quit ScreenPlay")
            onTriggered: App.exit()
        }
    }
}
