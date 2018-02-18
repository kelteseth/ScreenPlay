import QtQuick 2.9
import net.aimber.screenplaysdk 1.0

Rectangle {
    color: "gray"
    anchors.fill: parent
    property string tmpVideoPath

    ScreenPlaySDK {
        contentType: "wallpaper"

        onIncommingMessageError: {
            //name.text = msg
        }

        onSdkConnected: {
            //name.text = "connected"
        }

        onSdkDisconnected: {
            //name.text = "disconnected"
            screenVideo.state = "destroy"
            mainwindow.destroyThis()
        }
    }


    Connections {
        target: mainwindow
        onPlayVideo: {
            screenVideo.videoPath = path
        }
    }

    ScreenVideo {
        id:screenVideo
    }
}
