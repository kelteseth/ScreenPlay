import QtQuick 2.9
import QtAV 1.7

Rectangle {
    id: screenVideoPlayer
    color: "black"
    anchors.fill: parent

    property string videoPath
    property string decoder:  "DXVA" //["CUDA", "VAAPI", "D3D11", "DXVA", "FFmpeg"]
    property string fillMode
    onFillModeChanged: {
        if(fillMode === "Stretch"){
            videoOut.fillMode = VideoOutput.Stretch
        } else if(fillMode === "PreserveAspectFit"){
            videoOut.fillMode = VideoOutput.PreserveAspectFit
        } else if(fillMode === "PreserveAspectCrop"){
            videoOut.fillMode = VideoOutput.PreserveAspectCrop
        }
    }

    property bool loops: true
    property real volume: 0

    onVideoPathChanged: {
        player.source = Qt.resolvedUrl("file:///" + videoPath)
        player.play()
    }

    property bool isPlaying: true

    VideoOutput2 {
        id: videoOut
        anchors.fill: parent
        source: player
        opengl: true
        fillMode: VideoOutput.Stretch
    }

    MediaPlayer {
        id: player
        videoCodecPriority: screenVideoPlayer.decoder
        loops: MediaPlayer.Infinite
        volume: screenVideoPlayer.volume
        onPlaybackStateChanged: {
            if (player.playbackState === MediaPlayer.PlayingState) {
                state = "playing"
                mainwindow.init()
            }
        }
    }

    states: [
        State {
            name: "playing"
            PropertyChanges {
                target: player
                volume: screenVideoPlayer.volume
            }
        },
        State {
            name: "destroy"
            PropertyChanges {
                target: player
                volume: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "playing"
            reversible: true

            NumberAnimation {
                target: player
                property: "volume"
                duration: 1000
            }
        },
        Transition {
            from: "playing"
            to: "destroy"
            reversible: true

            NumberAnimation {
                target: player
                property: "volume"
                duration: 200
            }
        }
    ]
}
