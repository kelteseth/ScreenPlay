import QtQuick 2.9
import net.aimber.screenplaysdk 1.0

Rectangle {
    color: "gray"
    anchors.fill: parent
    property string tmpVideoPath
    //Component.onCompleted: mainwindow.init()

    ScreenPlaySDK {
        contentType: "wallpaper"

        onIncommingMessageError: {

        }

        onSdkConnected: {

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
