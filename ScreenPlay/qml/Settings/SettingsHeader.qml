import QtQuick 2.9
import QtGraphicalEffects 1.0

Item {
    id: settingsHeader
    state: "out"
    Component.onCompleted: state = "in"
    property color background: "#FFAB00"
    property string text: "HEADLINE"
    property url image: "qrc:/assets/icons/icon_settings.svg"
    height: 50
    width: parent.width

    Rectangle {
        id:radiusWorkaround
        height:5
        radius: 4
        color: settingsHeader.background
        anchors{
            top:parent.top
            right: parent.right
            left:parent.left
        }
    }

    Rectangle {
        color: settingsHeader.background
        height:47
        anchors{
            top:radiusWorkaround.bottom
            topMargin: -2
            right: parent.right
            left:parent.left
        }


        Item {
            anchors {
                fill: parent
                margins: 10
                leftMargin: 20
            }


            Image {
                id: imgIcon
                source: settingsHeader.image
                height: 20
                width: 20
                sourceSize: Qt.size(20, 20)
                anchors{
                    top:parent.top
                    topMargin: 3
                    left:parent.left
                    leftMargin: 0
                }
            }
            ColorOverlay {
                id:iconColorOverlay
                anchors.fill: imgIcon
                source: imgIcon
                color: "#ffffff"

            }
            Text {
                id: txtHeadline
                text: settingsHeader.text
                font.pixelSize: 18
                color: "white"
                renderType: Text.NativeRendering
                font.family: "Roboto"
                anchors{
                    top:parent.top
                    topMargin: 0
                    left:parent.left
                    leftMargin: 30
                }
            }
        }
    }
    states: [
        State {
            name: "out"

            PropertyChanges {
                target: imgIcon
                anchors.leftMargin: -50
                opacity: 0
            }

            PropertyChanges {
                target: txtHeadline
                anchors.topMargin: 50
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: imgIcon
                anchors.leftMargin: 3
                opacity: 1
            }

            PropertyChanges {
                target: txtHeadline
                anchors.topMargin: 0
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "in"
            to: "out"
            reversible: true

            NumberAnimation {
                targets: [imgIcon,txtHeadline]
                properties: "opacity, anchors.topMargin, anchors.leftMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
