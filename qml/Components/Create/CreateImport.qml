import QtQuick 2.7
import QtAV 1.7
Item {
    id:createImport
    anchors.fill: parent
    state: "out"
    Component.onCompleted: state =  "in"
    property url file
    onFileChanged: {
        timerSource.start()
    }
    Timer {
        id:timerSource
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            var tmp = Qt.resolvedUrl(file).toString();
            player.source = tmp
            player.play()
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: 800
        height: 500
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
            videoCodecPriority: ["CUDA","D3D11", "DXVA", "VAAPI","FFmpeg"] //

            autoPlay: true
            loops: MediaPlayer.Infinite
            volume: 0
        }

    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: createImport
                opacity: 0

            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: createImport
                opacity:1
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true
            PropertyAnimation{
                duration: 200
            }
        }
    ]
}
