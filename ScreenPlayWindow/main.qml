import QtQuick 2.9
import net.aimber.screenplaysdk 1.0

Rectangle {
    color: "gray"
    anchors.fill: parent
    property string tmpVideoPath
    property var jsonProjectFile
   Component.onCompleted: {
//        jsonProjectFile = JSON.parse(mainwindow.projectConfig)

//        if(jsonProjectFile.type === "qmlScene"){

//        }
    }

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
            //screenVideoLoader.setSource("qrc:/ScreenVideo.qml", {videoPath: path})
        }
        onPlayQmlScene: {
            var tmp = Qt.resolvedUrl("file:///" + file)
            print(tmp)
            sceneLoader.setSource(tmp)
            mainwindow.init()
        }

    }

    ScreenVideo {
        id:screenVideo
    }
//    Loader {
//        id:screenVideoLoader
//        anchors.fill: parent
//    }

    Loader {
        id:sceneLoader
        anchors.fill: parent
        z:11
    }
}
