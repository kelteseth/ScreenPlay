import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0

import ScreenPlay 1.0

import "../Workshop"

Item {
    id: createWallpaper
    state: "out"

    Component.onCompleted: createWallpaper.state = "in"

    signal videoImportConvertFileSelected(var videoFile)
    signal projectFileSelected(var projectFile)

    Text {
        id: txtHeadline
        text: qsTr("Import Content")
        opacity: 0
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        color: "white"
        font.pointSize: 21
        font.family: ScreenPlay.settings.font
        font.weight: Font.Thin
    }

    Item {
        id: wrapperImportVideo
        width: parent.width
        height: 275
        anchors {
            top: parent.top
            topMargin: 50
            left: parent.left
        }

        Rectangle {
            id: importVideoBg
            color: Material.background

            radius: 3
            z: 10
            anchors {
                fill: parent
                margins: 10
            }

            Image {
                id: imgUploadImportVideo
                source: "qrc:/assets/icons/icon_movie.svg"
                height: 120
                width: 120
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    topMargin: 50
                }
                sourceSize: Qt.size(width, height)
            }
            ColorOverlay {
                anchors.fill: imgUploadImportVideo
                source: imgUploadImportVideo
                color: "#C6C6C6"
            }

            Button {
                text: qsTr("Import video")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
                Material.background: Material.accent
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_upload.svg"
                icon.color: "white"
                icon.width: 16
                font.family: ScreenPlay.settings.font
                icon.height: 16
                onClicked: fileDialogImportVideo.open()
            }

            FileDialog {
                id: fileDialogImportVideo
                nameFilters: ["Video files (*.mp4  *.mpg *.mp2 *.mpeg *.ogv *.ogg *.avi *.wmv *.m4v *.3gp *.flv)"]

                onAccepted: {
                    videoImportConvertFileSelected(
                                fileDialogImportVideo.currentFile)
                }
            }
        }

        RectangularGlow {
            id: effect
            anchors {
                top: importVideoBg.top
                topMargin: 3
                left: importVideoBg.left
                right: importVideoBg.right
            }
            height: importVideoBg.height
            width: importVideoBg.width
            glowRadius: 3
            spread: 0.2
            color: "black"
            opacity: 0
            cornerRadius: 15
        }
    }


    Item {
        id: wrapperUploadProject
        width: parent.width
        height: 120
        visible: ScreenPlay.settings.steamVersion

        anchors {
            top: wrapperImportVideo.bottom
            left: parent.left
        }

        RectangularGlow {
            id: effect3
            width: importVideoBg3.width
            height: importVideoBg3.height
            anchors {
                top: importVideoBg3.top
                topMargin: 3
                left: importVideoBg3.left
                right: importVideoBg3.right
            }
            color: "#000000"
            glowRadius: 3
            spread: 0.2
            cornerRadius: 15
            opacity: 0
        }

        Rectangle {
            id: importVideoBg3
            radius: 3
            color: Material.background

            z: 10
            anchors {
                fill: parent
                margins: 10
            }

            Button {
                text: qsTr("Upload Exsisting Project to Steam")
                anchors.centerIn: parent
                Material.background: Material.accent
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_steam.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                font.family: ScreenPlay.settings.font
                onClicked: {
                    ScreenPlay.util.requestNavigation("Workshop")
                }
            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: wrapperUploadProject
                opacity: 0
                anchors.leftMargin: 300
            }
            PropertyChanges {
                target: wrapperImportVideo
                anchors.leftMargin: 800
                opacity: 0
            }
            PropertyChanges {
                target: effect3
                opacity: 0
            }
            PropertyChanges {
                target: effect
                opacity: 0
            }
            PropertyChanges {
                target: txtHeadline
                opacity: 0
                anchors.topMargin: -100
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: wrapperUploadProject
                opacity: 1
                anchors.leftMargin: 0
            }
            PropertyChanges {
                target: wrapperImportVideo
                anchors.leftMargin: 0
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: effect3
                opacity: .4
            }
            PropertyChanges {
                target: txtHeadline
                opacity: 1
                anchors.topMargin: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true

            ParallelAnimation {
                PropertyAnimation {
                    targets: [wrapperImportVideo,  wrapperUploadProject]
                    duration: 500
                    properties: "opacity, anchors.leftMargin"
                    easing.type: Easing.InOutQuart
                }
                SequentialAnimation {

                    PauseAnimation {
                        duration: 200
                    }
                    PropertyAnimation {
                        targets: [effect,  effect3]
                        duration: 500
                        property: "opacity"
                        easing.type: Easing.InOutQuart
                    }
                }
            }
            PropertyAnimation {
                targets: [txtHeadline]
                duration: 400
                properties: "opacity, anchors.topMargin"
                easing.type: Easing.InOutQuart
            }
        }
    ]
}












/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
