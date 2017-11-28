import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0

Item {
    id: createWallpaper
    state: "in"

    //Component.onCompleted: state = "in"
    signal fileSelected(var file)

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
    Rectangle {
        id: bg
        height: parent.height
        width: parent.width
        radius: 3
        z: 10
        anchors {
            top: parent.top
            topMargin: 50
            left: parent.left
            leftMargin: 0
        }

        Image {
            id: imgUpload
            source: "qrc:/assets/icons/icon_upload.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 80
            }
        }
        Text {
            id: txtDragAndDrop
            text: qsTr("Drag n' Drop")
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
                    target: bg
                    duration: 300
                    properties: "opacity, anchors.leftMargin"
                }

//                PropertyAnimation {
//                    target: effect
//                    duration: 1000
//                    properties: "opacity, visible, color"
//                }
            }
        }
    ]
}
