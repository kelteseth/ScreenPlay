import QtQuick 2.9
import QtGraphicalEffects 1.0

Item {
    id: btnEmpty
    height: 75
    anchors {
        right: parent.right
        left: parent.left
    }

    property string text
    signal clicked
    property bool buttonActive: true
    property string imgSource: "qrc:/assets/icons/icon_library_music.svg"


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
        opacity: .4
        cornerRadius: 15
    }

    Rectangle {
        id:bg
        width: parent.width
        anchors {
            top:parent.top
            right:parent.right
            bottom:parent.bottom
            left:parent.left
            margins: 10
        }

        radius: 3
        Image {
            id:imgIcon
            width: 30
            height: 30
            sourceSize: Qt.size(30,30)
            source: imgSource
            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }
        ColorOverlay{
            color: "gray"
            source: imgIcon
            anchors.fill: imgIcon
        }

        Text {
            id: name
            text: btnEmpty.text
            font.family: "Roboto"
            renderType: Text.NativeRendering
            font.pixelSize: 18
            color: "gray"
            anchors {
                left: imgIcon.right
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }
        Text {

            text: ">"
            font.family: "Roboto"
            renderType: Text.NativeRendering
            font.pixelSize: 24
            color: "#b9b9b9"
            anchors {
                right: parent.right
                rightMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: buttonActive
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                btnEmpty.clicked()
            }
        }
    }
}
