import QtQuick 2.9
import QtGraphicalEffects 1.0

Item {
    id: btnEmpty
    height: 63
    anchors {
        right: parent.right
        left: parent.left
    }

    property string text
    signal clicked
    property bool buttonActive: true
    property string imgSource: "qrc:/assets/icons/icon_library_music.svg"

    RectangularGlow {
        id: effectBtnEmpty
        anchors {
            top: btnEmpty.top
        }

        height: btnEmpty.height
        width: btnEmpty.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.2
        cornerRadius: 15
    }

    Rectangle {
        width: parent.width
        height: 60
        anchors.top: parent.top
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

            }
        }
    }
}
