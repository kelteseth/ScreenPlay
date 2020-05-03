import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0
import ScreenPlay 1.0

Item {
    id: btnEmpty
    height: 75
    state: "out"
    anchors {
        right: parent.right
        left: parent.left
    }

    Component.onCompleted: btnEmpty.state = "in"
    property int animOffset: 0
    property string text
    property bool buttonActive: true
    property string imgSource: "qrc:/assets/icons/icon_library_music.svg"

    signal clicked

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
        cornerRadius: 15
    }

    Rectangle {
        id: bg
        color: Material.background
        height: parent.height - 10
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            margins: 10
        }

        radius: 3
        Image {
            id: imgIcon
            width: 30
            height: 30
            sourceSize: Qt.size(30, 30)
            source: imgSource
            opacity: buttonActive ? 1 : .25
            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }
        ColorOverlay {
            color: "gray"
            source: imgIcon
            opacity: buttonActive ? 1 : .25
            anchors.fill: imgIcon
        }

        Text {
            id: name
            text: btnEmpty.text
            font.family: ScreenPlay.settings.font
            opacity: buttonActive ? 1 : .25
            font.pointSize: 14
            color: Material.theme === Material.Light ? Qt.lighter(Material.foreground) : Qt.darker(Material.foreground)
            wrapMode:  Text.WrapAnywhere
            maximumLineCount: 1

            anchors {
                left: imgIcon.right
                leftMargin: 20
                verticalCenter: parent.verticalCenter
                right:parent.right
                rightMargin: 40
            }
        }
        Text {
            text: ">"
            font.family: ScreenPlay.settings.font
            font.pointSize: 18
            opacity: buttonActive ? 1 : .25
            color: Material.theme === Material.Light ? Qt.lighter(Material.foreground) : Qt.darker(Material.foreground)
            anchors {
                right: parent.right
                rightMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            id:ma
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if(buttonActive)
                    btnEmpty.clicked()
            }
            hoverEnabled: true
            ToolTip.visible: !buttonActive ? ma.containsMouse : false
            ToolTip.text: qsTr("Not yet implemented. Stay tuned!")
        }
    }

    states: [
        State {
            name: "out"

            PropertyChanges {
                target: bg
                anchors.topMargin: -30
                opacity: 0
            }

            PropertyChanges {
                target: effect
                opacity: 0
            }
        },
        State {
            name: "in"

            PropertyChanges {
                target: bg
                anchors.topMargin: 10
                opacity: 1
            }

            PropertyChanges {
                target: effect
                opacity: .4
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"

            SequentialAnimation {
                PauseAnimation {
                    duration: btnEmpty.animOffset
                }
                ParallelAnimation {
                    ParallelAnimation {

                        NumberAnimation {
                            target: bg
                            property: "opacity"
                            duration: 400
                            easing.type: Easing.InOutQuart
                        }

                        NumberAnimation {
                            target: bg
                            property: "anchors.topMargin"
                            easing.type: Easing.InOutQuart
                            duration: 400
                        }
                    }
                    SequentialAnimation {
                        PauseAnimation {
                            duration: 200
                        }
                        NumberAnimation {
                            target: effect
                            property: "opacity"
                            duration: 400
                            easing.type: Easing.InOutQuart
                        }
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "out"

            SequentialAnimation {
                PauseAnimation {
                    duration: btnEmpty.animOffset
                }
                ParallelAnimation {
                    ParallelAnimation {

                        PropertyAnimation {
                            target: bg
                            property: "opacity"
                            duration: 400
                            easing.type: Easing.InOutQuart
                        }

                        PropertyAnimation {
                            target: bg
                            property: "anchors.topMargin"
                            easing.type: Easing.InOutQuart
                            duration: 400
                        }
                    }
                    SequentialAnimation {
                        PauseAnimation {
                            duration: 200
                        }
                        PropertyAnimation {
                            target: effect
                            property: "opacity"
                            duration: 400
                            easing.type: Easing.InOutQuart
                        }
                    }
                }
            }
        }
    ]
}
