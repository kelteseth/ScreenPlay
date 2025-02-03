import QtQuick
import QtQuick.Controls
import ScreenPlayCore
import ScreenPlay

Dialog {
    id: dialog
    width: 1200
    height: 700
    dim: true
    anchors.centerIn: Overlay.overlay
    modal: true
    focus: true

    property alias message: messageText.text
    Text {
        id: messageText
        color: Material.primaryTextColor
    }
    onOpened: wrapper.state = "in"
    onClosed: wrapper.state = "out"
    Rectangle {
        id: wrapper
        state: "out"
        color: Material.backgroundColor

        anchors {
            fill: parent
        }
        clip: true

        Image {
            id: imgBg
            source: "qrc:/qt/qml/ScreenPlay/assets/images/Intro.png"
            anchors.fill: parent
        }

        Item {
            height: parent.height
            anchors {
                top: parent.top
                topMargin: parent.height * 0.5 + 50
                right: parent.right
                left: parent.left
            }

            Image {
                id: imgShine

                source: "qrc:/qt/qml/ScreenPlay/assets/images/Intro_shine.png"
                height: 1753
                width: 1753
                opacity: 0
                anchors.centerIn: parent

                RotationAnimator {
                    target: imgShine
                    from: 0
                    to: 360
                    duration: 1000000
                    running: true
                    loops: Animation.Infinite
                }
            }
        }

        Image {
            id: imgLogo

            source: "qrc:/qt/qml/ScreenPlay/assets/images/pro_version.png"
            width: 539
            height: 148
            sourceSize: Qt.size(width, height)

            anchors {
                top: parent.top
                topMargin: -200
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            id: txtHeadline

            y: 80
            text: qsTr("✅ 3D Wallpaper\n✅ Wallpaper Timeline")
            font.family: App.settings.font
            font.capitalization: Font.Capitalize
            wrapMode: Text.WordWrap
            color: "white"
            font.pointSize: 21
            horizontalAlignment: Text.AlignHCenter

            anchors {
                right: parent.right
                left: parent.left
                top: parent.top
            }
        }

        Image {
            id: imgPC

            source: "qrc:/qt/qml/ScreenPlay/assets/images/Intro_PC.png"
            width: 500 * 0.8
            height: 500 * 0.8
            sourceSize: Qt.size(width, height)

            anchors {
                top: parent.top
                topMargin: 50
                horizontalCenter: parent.horizontalCenter
            }
        }

        Button {
            id: btnWorkshop

            text: qsTr("Get ScreenPlay Pro")
            Material.background: Material.color(Material.Orange)
            Material.foreground: "white"
            font.pointSize: 16
            width: implicitWidth + 20
            height: implicitHeight + 10
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_community.svg"
            icon.width: 18
            icon.height: 18
            onClicked: {
                if (App.globalVariables.isSteamVersion()) {
                    Qt.openUrlExternally("https://forum.screen-play.app/");
                } else {
                    Qt.openUrlExternally("https://forum.screen-play.app/");
                }
            }

            anchors {
                bottom: parent.bottom
                bottomMargin: -100
                horizontalCenter: parent.horizontalCenter
            }
        }

        ToolButton {
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_close.svg"
            icon.color: "transparent"
            onClicked: dialog.close()
            anchors {
                top: parent.top
                right: parent.right
            }
        }

        states: [
            State {
                name: "out"

                PropertyChanges {
                    target: imgBg
                    opacity: 0
                }

                PropertyChanges {
                    target: imgShine
                    opacity: 0
                }

                PropertyChanges {
                    target: imgPC
                    opacity: 0
                    anchors.topMargin: -500
                }

                PropertyChanges {
                    target: imgLogo
                    opacity: 1
                    anchors.topMargin: -500
                }

                PropertyChanges {
                    target: txtHeadline
                    opacity: 0
                    anchors.topMargin: 0
                }

                PropertyChanges {
                    target: btnWorkshop
                    anchors.bottomMargin: -100
                }
            },
            State {
                name: "in"

                PropertyChanges {
                    target: imgBg
                    opacity: 1
                }

                PropertyChanges {
                    target: imgShine
                    opacity: 0.5
                }

                PropertyChanges {
                    target: imgPC
                    opacity: 1
                    anchors.topMargin: 250
                }

                PropertyChanges {
                    target: imgLogo
                    opacity: 1
                    anchors.topMargin: 50
                }

                PropertyChanges {
                    target: txtHeadline
                    color: "#ffffff"
                    opacity: 1
                    anchors.topMargin: 250
                }

                PropertyChanges {
                    target: btnWorkshop
                    anchors.bottomMargin: 50
                }
            }
        ]
        transitions: [
            Transition {
                from: "out"
                to: "in"
                reversible: true

                ParallelAnimation {
                    PropertyAnimation {
                        targets: imgBg
                        property: "opacity"
                        duration: 400
                    }

                    PropertyAnimation {
                        targets: imgShine
                        property: "opacity"
                        duration: 600
                    }
                }

                SequentialAnimation {
                    PauseAnimation {
                        duration: 200
                    }

                    PropertyAnimation {
                        targets: imgPC
                        property: "topMargin"
                        duration: 700
                        easing.type: Easing.OutBack
                    }

                    PropertyAnimation {
                        targets: imgPC
                        property: "opacity"
                        duration: 600
                        easing.type: Easing.OutBack
                    }
                }

                ParallelAnimation {

                    PropertyAnimation {
                        targets: imgLogo
                        property: "opacity"
                        duration: 600
                        easing.type: Easing.InOutExpo
                    }

                    PropertyAnimation {
                        targets: imgLogo
                        property: "topMargin"
                        duration: 1000
                        easing.type: Easing.InOutExpo
                    }
                }

                SequentialAnimation {
                    PauseAnimation {
                        duration: 400
                    }

                    PropertyAnimation {
                        targets: txtHeadline
                        properties: "topMargin, opacity"
                        duration: 1100
                        easing.type: Easing.InOutExpo
                    }
                }

                SequentialAnimation {
                    PauseAnimation {
                        duration: 600
                    }

                    PropertyAnimation {
                        targets: btnWorkshop
                        properties: "bottomMargin"
                        duration: 1500
                        easing.type: Easing.OutBack
                    }
                }
            }
        ]
    }
}
