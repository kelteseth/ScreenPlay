import QtQuick 2.7
import QtAV 1.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {
    id: createImport
    anchors.fill: parent
    state: "out"
    Component.onCompleted: state = "in"
    property url file
    onFileChanged: {
        timerSource.start()
    }
    Timer {
        id: timerSource
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            var tmp = Qt.resolvedUrl(file).toString()
            player.source = tmp
            player.play()
        }
    }

    RectangularGlow {
        id: effect
        anchors {
            top: videoOutWrapper.top
            left: videoOutWrapper.left
            right: videoOutWrapper.right
            topMargin: 3
        }

        height: videoOutWrapper.height
        width: videoOutWrapper.width
        cached: true
        z: 9
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Rectangle {
        id: videoOutWrapper
        width: 500
        height: 300
        z: 12
        color: "black"
        anchors {
            top: parent.top
            topMargin: 150
            horizontalCenter: parent.horizontalCenter
        }

        VideoOutput2 {
            id: videoOut
            z: 13
            anchors.fill: parent
            source: player
            opengl: true
            fillMode: VideoOutput.Stretch
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPositionChanged: {
                    player.seek((mouseX / videoOut.width) * player.duration)
                    //print(mouseX / videoOut.width)
                    //player.pause()
                    busyIndicator.visible = true
                }
                onClicked: {
                    //player.seek((mouseX / videoOut.width) * player.duration)
                    //player.position = player.metaData
                }
            }

            BusyIndicator {
                id: busyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                visible: false
            }
        }
    }

    MediaPlayer {
        id: player
        videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
        autoPlay: true
        loops: MediaPlayer.Infinite
        volume: 0
        onSeekFinished: {
            busyIndicator.visible = false
        }
    }
    RectangularGlow {
        id: effect2
        anchors {
            top: contentWrapper.top
            left: contentWrapper.left
            right: contentWrapper.right
            topMargin: 3
        }

        height: contentWrapper.height
        width: contentWrapper.width
        cached: true
        z: 9
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }
    Rectangle {
        id: contentWrapper
        width: 800
        height: 280
        z:10
        radius: 4
        anchors {
            top: parent.top
            topMargin: 300
            horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: qsTr("Import")
            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
        }
    }



    states: [
        State {
            name: "out"
            PropertyChanges {
                target: createImport
                opacity: 0
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 0
                opacity: 0
                anchors.topMargin: 50
            }
            PropertyChanges {
                target: effect
                opacity: 0
                color: "transparent"
            }
            PropertyChanges {
                target: contentWrapper
                anchors.topMargin: -300
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: createImport
                opacity: 1
            }
            PropertyChanges {
                target: videoOut
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
                color: "black"
            }
            PropertyChanges {
                target: contentWrapper
                opacity: 1
                anchors.topMargin: 300
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 12
                opacity: 1
                anchors.topMargin: 50
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true
            SequentialAnimation {
                PauseAnimation {
                    duration: 500
                }
                ParallelAnimation {
                    PropertyAnimation {
                        duration: 300
                        target: contentWrapper
                        property: "opacity"
                    }
                    PropertyAnimation {
                        duration: 300
                        target: contentWrapper
                        property: "anchors.topMargin"
                        easing.type: Easing.InOutQuad
                    }
                }
                ParallelAnimation {
                    PropertyAnimation {
                        duration: 250
                        target: videoOutWrapper
                        property: "opacity"
                    }
                    PropertyAnimation {
                        duration: 250
                        target: effect
                        properties: "color, opacity"
                    }
                }
            }
        }
    ]
}
