import QtQuick 2.0
import QtQuick.Controls 2.12
import QtWebSockets 1.1
import QtWebEngine 1.8
import QtMultimedia 5.12


Rectangle {
    id: root
    color: "black"
    anchors.fill: parent

    property string fullContentPath
    property real volume: 1
    property string fillMode: "Cover"
    property string type
    property string projectSourceFileAbsolute
    property bool loops: true

    MediaPlayer {
        id: player1
        volume: root.volume
        source: root.projectSourceFileAbsolute
        onStopped: {
                videoOutput1.visible = false
                videoOutput2.visible = true
                if(player2.source != root.projectSourceFileAbsolute){
                    player2.source = root.projectSourceFileAbsolute
                }
                player1.play()
                player1.pause()
                player2.play()
        }
    }

    MediaPlayer {
        id: player2
        volume: root.volume
        onStopped: {
            videoOutput2.visible = false
            videoOutput1.visible = true
            player2.play()
            player2.pause()
            player1.play()
        }
    }


    VideoOutput {
        id: videoOutput1
        fillMode: VideoOutput.PreserveAspectCrop
        anchors.fill: parent
        source: player1
    }

    VideoOutput {
        id: videoOutput2
        fillMode: VideoOutput.PreserveAspectCrop
        anchors.fill: parent
        source: player2
    }

    Component.onCompleted: {
        print("test")
    }

    WebSocket {
        id: socket

        url: "ws://127.0.0.1:16395"
        active: true
        onStatusChanged: {
             if (socket.status === WebSocket.Open)
                socket.sendTextMessage("Hello World from QML wallpaper")
        }

        onTextMessageReceived: (message)=> {
            socket.sendTextMessage(message)
            print(message)

            var obj = JSON.parse(message)
            wp.connected = true

            if (obj.command === "replace") {
                socket.sendTextMessage("replace")
                root.type = obj.type
                root.fillMode = obj.fillMode
                root.volume = obj.volume
                root.projectSourceFileAbsolute = "file://" + obj.absolutePath + "/" + obj.file
                print("got: " + root.projectSourceFileAbsolute)
                //mediaplayer.source = root.projectSourceFileAbsolute
                //mediaplayer.play()
                //videplayer.source = root.projectSourceFileAbsolute
                //playlist.addItem(root.projectSourceFileAbsolute)
                //playlist.addItem(root.projectSourceFileAbsolute)
                //videplayer.play()

                videoOutput2.visible = false
                videoOutput1.visible = true

                //if(wallpaper.configuration.DualPlayback){
                player2.source = root.projectSourceFileAbsolute
                player2.play()
                player2.pause()
                //}
                player1.play()
           }
        }
    }
}
