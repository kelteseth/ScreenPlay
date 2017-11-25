import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    id:btnEmpty
    height: 63
    anchors {
        right:parent.right
        left:parent.left
    }

    property string text
    signal clicked

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
        Rectangle {
            id: imgIconEmpty
            width: 30
            height: 30
            color: "gray"
            radius: 30
            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }

        Text {
            id: name
            text: btnEmpty.text
            font.family: font_Roboto_Regular.name
            renderType: Text.NativeRendering
            font.pixelSize: 18
            color: "gray"
            anchors {
                left: imgIconEmpty.right
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                clicked()
            }
        }
    }

}

