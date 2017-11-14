import QtQuick 2.7
import QtAV 1.07

Rectangle {
    id:screenVideo

    Component.onCompleted: {
        player.source = Qt.resolvedUrl("file:///" + wallpaper.absoluteFilePath)
        player.play()
    }

    VideoOutput2 {
        id: videoOut
        anchors.fill: parent
        source: player
        opengl: true
        fillMode: VideoOutput.Stretch
    }

    MediaPlayer {
        id: player
        onPlaying: screenVideo.state = "playing"
        videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
        loops: MediaPlayer.Infinite
    }

    Rectangle {
        id: curtain
        anchors.fill: parent
        color: "black"
        opacity: 1
        z:99
    }

    states: [
        State {
            name: "playing"
            PropertyChanges {
                target: curtain
                opacity:0
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "playing"

            NumberAnimation {
                target: curtain
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
