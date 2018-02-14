import QtQuick 2.9
import net.aimber.screenplaysdk 1.0

Rectangle {
    color: "gray"
    anchors.fill: parent


    ScreenPlaySDK {
        contentType: "wallpaper"

        onIncommingMessageError: {
            name.text = msg
        }

        onSdkConnected: {
            name.text = "connected"
        }

        onSdkDisconnected:  {
            name.text = "disconnected"
            //mainwindow.destroyThis()
        }
    }

    Connections {
        target: mainwindow
        onPlayVideo:{
            loaderVideo.active = true
            print(path)
            //loaderVideo.item.videoPath = path

        }
    }

    Loader {
        id:loaderVideo
        anchors.fill: parent
        asynchronous: true
        active: false
        source: "qrc:/ScreenVideo.qml"
        onStateChanged: {
            if(status == Loader.Ready){
                loaderVideo.item.isPlaying = true
            }
        }
    }
}
