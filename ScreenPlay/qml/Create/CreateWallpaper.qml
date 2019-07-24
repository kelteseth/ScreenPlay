import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0
import QtQuick.Controls.Material 2.2

import "../Workshop"

Item {
    id: createWallpaper
    state: "out"

    Component.onCompleted: createWallpaper.state = "in"

    signal videoImportConvertFileSelected(var videoFile)
    signal projectFileSelected(var projectFile)

    WorkshopLoader {
        id: wl
    }
    Text {
        id: txtHeadline
        text: qsTr("Import Wallpaper")
        opacity: 0
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        color: "white"
        font.pointSize: 21

        font.family: "Roboto"
        font.weight: Font.Thin
    }

    Item {
        id: wrapperImportVideo
        width: parent.width * .48
        height: 275
        anchors {
            top: parent.top
            topMargin: 50
            left: parent.left
        }

        Rectangle {
            id: importVideoBg
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
                Material.background: Material.Orange
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_upload.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: fileDialogImportVideo.open()
            }

            FileDialog {
                id: fileDialogImportVideo

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
        id: wrapperConvertVideo
        width: parent.width * .48
        height: 275
        anchors {
            top: parent.top
            topMargin: 50
            left: wrapperImportVideo.right
            leftMargin: 20
        }

        Rectangle {
            id: convertVideoBg
            radius: 3
            z: 10
            anchors {
                fill: parent
                margins: 10
            }

            Image {
                id: imgUploadImportVideo2
                source: "qrc:/assets/icons/icon_scene.svg"
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
                anchors.fill: imgUploadImportVideo2
                source: imgUploadImportVideo2
                color: "#C6C6C6"
            }

            Button {
                text: qsTr("Import ThreeJs Scene")
                enabled: false
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
                Material.background: Material.Orange
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_upload.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: fileDialogImportProject.open()
            }

            FileDialog {
                id: fileDialogImportProject
                nameFilters: ["ThreeJS Scene files(scene.pkg)"]
                onAccepted: {
                    projectFileSelected(
                                fileDialogImportProject.currentFile)
                }
            }
        }

        RectangularGlow {
            id: effect2
            anchors {
                top: convertVideoBg.top
                topMargin: 3
                left: convertVideoBg.left
                right: convertVideoBg.right
            }
            height: convertVideoBg.height
            width: convertVideoBg.width
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
        visible: wl.available

        anchors {
            top: wrapperConvertVideo.bottom
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
            z: 10
            anchors {
                fill: parent
                margins: 10
            }

            Button {
                text: qsTr("Upload Exsisting Project to Steam")
                anchors.centerIn: parent
                Material.background: Material.Orange
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_steam.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: {
                    utility.requestNavigation("Workshop")
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
                target: wrapperConvertVideo
                anchors.leftMargin: 800
                opacity: 0
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
                target: effect2
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
                target: wrapperConvertVideo
                anchors.leftMargin: 20
                opacity: 1
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
                target: effect2
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
                    targets: [wrapperImportVideo, wrapperConvertVideo, wrapperUploadProject]
                    duration: 500
                    properties: "opacity, anchors.leftMargin"
                    easing.type: Easing.InOutQuart
                }
                SequentialAnimation {

                    PauseAnimation {
                        duration: 200
                    }
                    PropertyAnimation {
                        targets: [effect, effect2, effect3]
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
