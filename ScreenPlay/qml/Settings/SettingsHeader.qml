import QtQuick 2.12
import QtQuick.Layouts 1.12
import Qt5Compat.GraphicalEffects
import ScreenPlay 1.0

Item {
    id: settingsHeader

    property color background: "#FFAB00"
    property string text: "HEADLINE"
    property url image: "qrc:/assets/icons/icon_settings.svg"

    state: "out"
    Component.onCompleted: state = "in"
    width: parent.width
    height: 70

    Rectangle {
        id: radiusWorkaround

        height: 5
        radius: 4
        color: settingsHeader.background

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

    }

    Rectangle {
        color: settingsHeader.background
        height: 47

        anchors {
            top: radiusWorkaround.bottom
            topMargin: -2
            right: parent.right
            left: parent.left
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

                anchors {
                    top: parent.top
                    topMargin: 3
                    left: parent.left
                    leftMargin: 0
                }

            }

            ColorOverlay {
                id: iconColorOverlay

                anchors.fill: imgIcon
                source: imgIcon
                color: "#ffffff"
            }

            Text {
                id: txtHeadline

                text: settingsHeader.text
                font.pointSize: 12
                color: "white"
                verticalAlignment: Text.AlignTop
                font.family: ScreenPlay.settings.font

                anchors {
                    top: parent.top
                    topMargin: 0
                    left: parent.left
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
                anchors.leftMargin: -10
                opacity: 0
            }

            PropertyChanges {
                target: txtHeadline
                anchors.topMargin: 10
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
                anchors.topMargin: 2
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
                targets: [imgIcon, txtHeadline]
                properties: "opacity, anchors.topMargin, anchors.leftMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }

        }
    ]
}
