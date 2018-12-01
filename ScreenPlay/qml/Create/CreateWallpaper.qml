import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0
import QtQuick.Controls.Material 2.2

Item {
    id: createWallpaper
    state: "in"

    signal videoFileSelected(var videoFile)
    signal projectFileSelected(var projectFile)

    Text {
        id: txtHeadline
        text: qsTr("Import Video Wallpaper")
        opacity: 0
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        color: "white"
        font.pixelSize: 28
        renderType: Text.NativeRendering
        font.family: "Roboto"
        font.weight: Font.Thin
    }

    Item {
        id: item1
        width: parent.width
        height: 250
        anchors.top: parent.top
        anchors.topMargin: 50

        Rectangle {
            id: bg
            x: 0
            y: 50
            height: parent.height
            width: parent.width
            radius: 3
            z: 10
            anchors {
                top: parent.top
                topMargin: 0
                left: parent.left
                leftMargin: 0
            }

            Image {
                id: imgUpload
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
                anchors.fill: imgUpload
                source: imgUpload
                color: "#C6C6C6"
            }

            Button {
                text: qsTr("Select File")
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
                onClicked: fileDialogOpenFile.open()
            }

            FileDialog {
                id: fileDialogOpenFile
                onAccepted: {
                    videoFileSelected(fileDialogOpenFile.currentFile)
                }
            }
        }

        RectangularGlow {
            id: effect
            anchors {
                top: bg.top
                topMargin: 3
                left: bg.left
                right: bg.right
            }
            height: bg.height
            width: bg.width
            glowRadius: 3
            spread: 0.2
            color: "black"
            opacity: 0
            visible: false
            cornerRadius: 15
        }
    }

    Item {
        id: item2
        width: parent.width
        height: 80
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        RectangularGlow {
            id: effect1
            width: bg1.width
            height: bg1.height
            anchors {
                top: bg1.top
                topMargin: 3
                left: bg1.left
                right: bg1.right
            }
            color: "#000000"
            glowRadius: 3
            spread: 0.2
            cornerRadius: 15
            opacity: 0
            visible: false
        }

        Rectangle {
            id: bg1
            width: parent.width
            height: parent.height
            radius: 3
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.topMargin: 0
            z: 10
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
                    fileDialogOpenProject.open()
                }
            }
            FileDialog {
                id: fileDialogOpenProject
                nameFilters: ["Project files (project.json)"]
                onAccepted: {
                    projectFileSelected(fileDialogOpenProject.currentFile)
                }
            }
        }
    }
    states: [
        State {
            name: "out"
            PropertyChanges {
                target: bg
                opacity: 0
                anchors.leftMargin: 300
            }
            PropertyChanges {
                target: effect
                opacity: 0
                visible: false
                color: "transparent"
            }
            PropertyChanges {
                target: effect1
                opacity: 0
                visible: false
                color: "transparent"
            }
            PropertyChanges {
                target: txtHeadline
                opacity: 0
            }

            PropertyChanges {
                target: bg1
                anchors.leftMargin: 800
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: bg
                opacity: 1
                anchors.leftMargin: 0
            }
            PropertyChanges {
                target: effect
                opacity: .3
                color: "black"
                visible: true
            }
            PropertyChanges {
                target: effect1
                opacity: .3
                color: "black"
                visible: true
            }
            PropertyChanges {
                target: txtHeadline
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "in"
            to: "out"
            reversible: true
            SequentialAnimation {
                PropertyAnimation {
                    targets: [bg, bg1]
                    duration: 300
                    properties: "opacity, anchors.leftMargin"
                }
            }
        }
    ]
}
