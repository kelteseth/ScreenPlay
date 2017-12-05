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
    property url videoFile: ""
    property url projectFile: ""

    Connections {
        target: steamWorkshop
        ignoreUnknownSignals: true
        onWorkshopCreationCopyVideo: {
            print("Copy video", sucessful)
        }
        onWorkshopCreationCopyImage: {
            print("Copy image", sucessful)
        }
        onWorkshopCreationCompleted: {
            print("Workshop Creation Complete", sucessful)
        }
        onLocalFileCopyCompleted: {
            print("Copy complete", sucessful)
        }
        onWorkshopCreationFolderDuplicate: {
            print("duplicate")
        }
    }


    Timer {
        id: timerUpload
        running: false
        triggeredOnStart: true
        repeat: true
        interval: 500
        onRunningChanged: print(timerUpload.running)
        onTriggered: {
            var status = steamWorkshop.getItemUpdateProcess()
            print(status)
            switch (status) {
            case 0:
                text2.text = "The item update handle was invalid, the job might be finished. Who knows..."
                break
            case 1:
                text2.text = "The item update is processing configuration data."
                break
            case 2:
                text2.text = "The item update is reading and processing content files."
                break
            case 3:
                text2.text = "The item update is uploading content changes to Steam."
                break
            case 4:
                text2.text = "The item update is uploading new preview file image."
                break
            case 5:
                text2.text = "The item update is committing all changes."
                timerUpload.running = false
                uploadCompleted()
                break
            default:
                break
            }
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
            topMargin: 30
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

            Rectangle {
                id: rectProgressBar
                height: 5
                color: "orange"
                anchors {
                    right: parent.right
                    rightMargin: videoOutWrapper.width
                    left: parent.left
                    leftMargin: 0
                    bottom: parent.bottom
                    bottomMargin: 0
                }
            }
        }
    }

    MediaPlayer {
        id: player
        videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
        autoPlay: false
        loops: MediaPlayer.Infinite
        volume: 0
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
                topMargin: 55
                right: parent.right
                rightMargin: 78
                bottom: parent.bottom
                bottomMargin: 71
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
                placeholderText: "Youtube Preview URL"
            }
            TextField {
                id: txtTags
                height: 60
                width: parent.width
                selectByMouse: true
                text: qsTr("")
                placeholderText: "Tags - Seperation between tags via , "
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
                steamWorkshop.submitWorkshopItem(txtTitle.text.toString(),
                                                 txtDescription.text.toString(
                                                     ), "english", "true", projectFile, videoFile)
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
