import QtQuick 2.0
import QtQuick.Controls 2.12
import QtWebSockets 1.1
import QtWebEngine 1.8
import QtMultimedia 5.12
import Qt.labs.settings 1.1


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

    function stop(){
        player1.stop()
        player2.stop()
        videoOutput1.visible = false
        videoOutput2.visible = false
        root.enabled = false
    }

    function play(){
        root.enabled = true
        videoOutput2.visible = false
        videoOutput1.visible = true

        //if(wallpaper.configuration.DualPlayback){
        player2.source = root.projectSourceFileAbsolute
        player2.play()
        player2.pause()
        //}
        player1.play()
    }

    MediaPlayer {
        id: player1
        volume: root.volume
        source: root.projectSourceFileAbsolute
        onStopped: {
            if(!root.enabled)
                return

                videoOutput1.visible = false
                videoOutput2.visible = true
                if(player2.source !== root.projectSourceFileAbsolute){
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
            if(!root.enabled)
                return
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



}
