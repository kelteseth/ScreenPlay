import QtQuick 2.0
import Qt.labs.platform 1.1
import ScreenPlay 1.0

SystemTrayIcon {
    id: root

    visible: true
    icon.source: "qrc:/assets/icons/app.ico"
    tooltip: qsTr("ScreenPlay - Double click to change you settings.")
    onActivated: (reason)=>{
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

    menu: Menu {
        MenuItem {
            text: qsTr("Open ScreenPlay")
            onTriggered: {
                window.show();
            }
        }

        MenuItem {
            id: miMuteAll

            property bool isMuted: true

            text: qsTr("Mute all")
            onTriggered: {
                if (miMuteAll.isMuted) {
                    isMuted = false;
                    miMuteAll.text = qsTr("Mute all");
                    ScreenPlay.screenPlayManager.setAllWallpaperValue("muted", "true");
                } else {
                    isMuted = true;
                    miMuteAll.text = qsTr("Unmute all");
                    ScreenPlay.screenPlayManager.setAllWallpaperValue("muted", "false");
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
                    ScreenPlay.screenPlayManager.setAllWallpaperValue("isPlaying", "true");
                } else {
                    isPlaying = true;
                    miStopAll.text = qsTr("Play all");
                    ScreenPlay.screenPlayManager.setAllWallpaperValue("isPlaying", "false");
                }
            }
        }

        MenuItem {
            text: qsTr("Quit")
            onTriggered: ScreenPlay.exit()
        }

    }

}
