import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {

    Text {
        id: text1
        text: qsTr("Import Video Wallpaper")
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
        }

        height: bg.height
        width: bg.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }
    Rectangle {
        id:bg
        anchors.fill: parent
        radius: 3
        anchors {
            top: parent.top
            topMargin: 60
        }

        Image {
            id: imgUpload
            source: "qrc:/assets/icons/icon_upload.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
                top:parent.top
                topMargin: 80
            }
        }
        Text {
            id: txtDragAndDrop
            text: qsTr("Drag n' Drop")
            anchors {
                horizontalCenter: parent.horizontalCenter
                top:imgUpload.bottom
                topMargin: 10
            }
            font.family: font_LibreBaskerville_Italic.name
            font.pixelSize: 18
            font.italic: true
            color: "#818181"
            FontLoader{
                id: font_LibreBaskerville_Italic
                source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
            }
        }
        Button {
            text: qsTr("Select File")
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom:parent.bottom
                bottomMargin: 20
            }
        }
    }
}
