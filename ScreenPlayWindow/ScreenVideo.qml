import QtQuick 2.9
import QtAV 1.7

Rectangle {
    id: screenVideoPlayer
    color: "black"
    anchors.fill: parent
    property string videoPath
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
        videoCodecPriority: ["CUDA", "VAAPI", "D3D11", "DXVA", "FFmpeg"]
        loops: MediaPlayer.Infinite
        volume: 0
        onStatusChanged: {
            if (player.status === MediaPlayer.Loaded) {
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
                volume: 1
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
                duration: 450
            }
        }
    ]
}
