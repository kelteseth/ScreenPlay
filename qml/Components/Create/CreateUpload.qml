import QtQuick 2.9
import QtAV 1.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0
import QtQuick.Controls.Material 2.2

import RemoteWorkshopCreationStatus 1.0

Item {
    id: createUpload
    anchors.fill: parent
    state: "out"
    Component.onCompleted: state = "in"
    property bool isVideoPlaying: true
    property url videoFile: ""
    property url projectFile: ""
    property var jsonProjectFile

    // First we parse the content of the project file
    // TODO: Implement parse error
    onProjectFileChanged: {
        jsonProjectFile = JSON.parse(screenPlaySettings.loadProject(
                                         projectFile))
    }

    // Now we add the content
    onJsonProjectFileChanged: {
        txtTitle.text = jsonProjectFile.title
        txtDescription.text = jsonProjectFile.description
    }

    Connections {
        target: steamWorkshop
        ignoreUnknownSignals: true
        onWorkshopItemCreated: {
            steamWorkshop.submitWorkshopItem(txtTitle.text.toString(),
                                             txtDescription.text.toString(),
                                             "english", "true", projectFile,
                                             videoFile)
        }
        onRemoteWorkshopCreationStatusChanged: {
            switch (status){
            case RemoteWorkshopCreationStatus.Started:
                timerUpload.start();
                break;
            }
        }
    }

    Timer {
        id: timerUpload
        running: false
        triggeredOnStart: true
        repeat: true
        interval: 200
        onTriggered: {
            var status = steamWorkshop.getItemUpdateProcess()
            print(status + steamWorkshop.steamWorkshop + " / " + steamWorkshop.bytesTotal)
            switch (status) {
            case 0:
                txtUploadStatus.text = "The item update handle was invalid, the job might be finished. Who knows..."
                break
            case 1:
                txtUploadStatus.text = "The item update is processing configuration data."
                break
            case 2:
                txtUploadStatus.text = "The item update is reading and processing content files."
                break
            case 3:
                txtUploadStatus.text = "The item update is uploading content changes to Steam."
                break
            case 4:
                txtUploadStatus.text = "The item update is uploading new preview file image."
                break
            case 5:
                txtUploadStatus.text = "The item update is committing all changes."
                timerUpload.running = false
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
                placeholderText: "Title"
            }
            TextField {
                id: txtDescription
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
            Material.background: Material.Orange
            Material.foreground: "white"
            onClicked: {
                createUpload.state = "upload"
                // We need to first create a workshop item
                // after we received the new item handle
                // from steam we can upload the content
                steamWorkshop.createWorkshopItem()
            }
        }
    }

    Item {
        id: uploadWrapper
        opacity: 0
        width: 600
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: videoOutWrapper.bottom
            topMargin: 20
            bottom: parent.bottom
        }

        Text {
            id: txtUploadStatus
            text: qsTr("Creating Workshop Item")
            color: "white"
            font.pixelSize: 16
            height: 30
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
            }
            font.family: font_Roboto_Regular.name
            renderType: Text.NativeRendering
        }

        ProgressBar {
            id: pbUpload
            value: .1
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: txtUploadStatus.bottom
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
            PropertyChanges {
                target: uploadWrapper
                opacity: 0
            }
        },
        State {
            name: "upload"
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
                opacity: 0
                color: "transparent"
            }
            PropertyChanges {
                target: contentWrapper
                opacity: 1
                anchors.topMargin: -500
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 12
                opacity: 1
                anchors.topMargin: 150
            }
            PropertyChanges {
                target: uploadWrapper
                opacity: 1
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
        },
        Transition {
            from: "in"
            to: "upload"
            reversible: true

            SequentialAnimation {
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
                        property: "topMargin"
                    }
                    PropertyAnimation {
                        duration: 250
                        target: effect
                        properties: "color, opacity"
                    }
                    PropertyAnimation {
                        duration: 250
                        target: uploadWrapper
                        property: "opacity"
                    }
                }
            }
        }
    ]
}
