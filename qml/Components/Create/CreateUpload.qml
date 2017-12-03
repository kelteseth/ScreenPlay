import QtQuick 2.7
import QtAV 1.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0

Item {
    id: createUpload
    anchors.fill: parent
    state: "out"
    Component.onCompleted: state = "in"
    property bool isVideoPlaying: true
    property url file
    onFileChanged: {
        timerSource.start()
    }

    Timer {
        id: timerSource
        interval: 1000

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
        width: 450
        height: 263
        z: 12
        color: "black"
        anchors {
            top: parent.top
            topMargin: 80
            horizontalCenter: parent.horizontalCenter
        }

        VideoOutput2 {
            id: videoOut
            z: 13
            anchors.fill: parent
            source: player
            opengl: true
            fillMode: VideoOutput.Stretch

            Image {
                id: imgPreview
                anchors.fill: parent
                opacity: 0
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
            pause()
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
        height: 350
        z: 10
        radius: 4
        anchors {
            top: parent.top
            topMargin: 250
            horizontalCenter: parent.horizontalCenter
        }

        Column {
            spacing: 20
            anchors {
                top: parent.top
                topMargin: 50
                right: parent.right
                rightMargin: 78
                bottom: parent.bottom
                bottomMargin: 50
                left: parent.left
                leftMargin: 78
            }

            TextField {
                id: txtTitle
                height: 60
                width: parent.width
                selectByMouse: true
                text: qsTr("")
                placeholderText: "Description"
            }

            TextField {
                id: txtYoutube
                height: 60
                width: parent.width
                selectByMouse: true
                text: qsTr("")
                placeholderText: "Youtube Preview"
            }
            TextField {
                id: txtTags
                height: 60
                width: parent.width
                selectByMouse: true
                text: qsTr("")
                placeholderText: "Tags"
            }

        }

        Button {
            text: qsTr("Upload to Steam")
            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {

            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: createUpload
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
                anchors.topMargin: -500
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: createUpload
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
                anchors.topMargin: 250
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 12
                opacity: 1
                anchors.topMargin: 20
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
