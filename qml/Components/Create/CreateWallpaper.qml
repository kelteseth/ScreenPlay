import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0

Item {
    id: createWallpaper
    state: "in"

    //Component.onCompleted: state = "in"
    signal fileSelected(var file)
    signal uploadExsisting(var file)

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
        font.family: font_Roboto_Regular.name

        FontLoader {
            id: font_Roboto_Regular
            source: "qrc:/assets/fonts/Roboto-Regular.ttf"
        }
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
                source: "qrc:/assets/icons/icon_upload.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    topMargin: 30
                }
            }
            Text {
                id: txtDragAndDrop
                text: qsTr("Drag n' Drop")
                visible: true
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: imgUpload.bottom
                    topMargin: 10
                }
                font.family: font_LibreBaskerville_Italic.name
                font.pixelSize: 18
                font.italic: true
                color: "#818181"
                FontLoader {
                    id: font_LibreBaskerville_Italic
                    source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
                }
            }
            Button {
                text: qsTr("Select File")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
                onClicked: fileDialogOpenFile.open()
            }

            FileDialog {
                id: fileDialogOpenFile
                nameFilters: ["Video files (*.mp4)"]
                onAccepted: {
                    fileSelected(fileDialogOpenFile.currentFile)
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
            z:9
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
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    fileDialogOpenFolder.open()
                }
            }
            FolderDialog {
                id: fileDialogOpenFolder
                onAccepted: {
                    uploadExsisting(fileDialogOpenFolder.currentFile)
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
                target: txtHeadline
                opacity: 0
            }

            PropertyChanges {
                target: bg1
                anchors.leftMargin: 800
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
                opacity: .4
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
                    targets: [bg,bg1]
                    duration: 300
                    properties: "opacity, anchors.leftMargin"
                }
            }
        }
    ]
}
