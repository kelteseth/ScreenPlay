import QtQuick 2.9
import net.aimber.screenplaysdk 1.0

Rectangle {
    id: root
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
        id: spSDK
        contentType: "ScreenPlayWindow"
        appID: mainwindow.appID

        onIncommingMessageError: {
            rctError.opacity = 1
            txtDebug.text = "ERROR: " + msg.toString()
        }
        onIncommingMessage: {
            var obj2 = 'import QtQuick 2.9; Item {Component.onCompleted: sceneLoader.item.' + key + ' = ' + value + '; }'
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
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

        onDecoderChanged:{
            screenVideo.decoder = decoder
        }
        onFillModeChanged:{
            screenVideo.fillMode = fillMode
        }
        onLoopsChanged:{
            screenVideo.loops = loops
        }
        onVolumeChanged:{
            screenVideo.volume = volume
        }
    }

    ScreenVideo {
        id: screenVideo
    }

    //    Loader {
    //        id:screenVideoLoader
    //        anchors.fill: parent
    //    }
    Loader {
        id: sceneLoader
        anchors.fill: parent
    }

    Rectangle {
        id: rctError
        opacity: 0
        height: 300
        width: 600
        anchors.centerIn: parent
        Text {
            id: txtDebug
            font.pixelSize: 32
            anchors.centerIn: parent
        }
    }
}
