import QtQuick 2.9
import QtAV 1.7
Rectangle {
    id:screenVideo

    Connections{
        target: wallpaper
        onVolumeChanged:{
            player.volume = wallpaper.volume
            print(player.volume)
        }
        onIsPlayingChanged:{
            if(wallpaper.isPlaying){
                player.play()
            } else {
                player.pause()
            }
        }
    }

    Component.onCompleted: {
        player.source = Qt.resolvedUrl("file:///" + wallpaper.absoluteFilePath)
        print("AAAA",Qt.resolvedUrl("file:///" + wallpaper.absoluteFilePath))
        player.play()
    }

    VideoOutput2 {
        id: videoOut
        anchors.fill: parent
        source: player
        fillMode: VideoOutput.Stretch
    }

    MediaPlayer {
        id: player
        onPlaying: screenVideo.state = "playing"
        videoCodecPriority: ["CUDA","FFmpeg", "D3D11", "DXVA", "VAAPI"] //,
        loops: MediaPlayer.Infinite
        volume: 0

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
            PropertyChanges {
                target: player
                volume: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "playing"

            OpacityAnimator {
                target: curtain
                duration: 500
                easing.type: Easing.InOutQuad
            }
            NumberAnimation{
                target: player
                property: "volume"
                duration: 500
            }
        }
    ]
}
