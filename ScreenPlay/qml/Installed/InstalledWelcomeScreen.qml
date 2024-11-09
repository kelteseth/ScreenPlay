import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlayCore

Item {
    id: installedUserHelper
    state: "out"
    Component.onCompleted: state = "in"

    anchors {
        fill: parent
    }

    Image {
        id: imgBg

        source: "qrc:/qml/ScreenPlayApp/assets/images/Intro.png"
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

            source: "qrc:/qml/ScreenPlayApp/assets/images/Intro_shine.png"
            height: 1753
            width: 1753
            opacity: 0
            anchors.centerIn: parent

            RotationAnimator {
                target: imgShine
                from: 0
                to: 360
                duration: 100000
                running: true
                loops: Animation.Infinite
            }
        }
    }

    Image {
        id: imgLogo

        source: "qrc:/qml/ScreenPlayApp/assets/images/Early_Access.png"
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
        text: App.globalVariables.isSteamVersion() ? qsTr("Get free Widgets and Wallpaper via the Steam Workshop") : qsTr("Get content via our forum")
        font.family: App.settings.font
        font.capitalization: Font.Capitalize
        wrapMode: Text.WordWrap
        color: "white"
        font.pointSize: 28
        horizontalAlignment: Text.AlignHCenter

        anchors {
            right: parent.right
            left: parent.left
            top: parent.top
        }
    }

    Image {
        id: imgPC

        source: "qrc:/qml/ScreenPlayApp/assets/images/Intro_PC.png"
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

        text: {
            if (App.globalVariables.isSteamVersion()) {
                return qsTr("Browse the Steam Workshop");
            } else {
                return qsTr("Open the ScreenPlay forum");
            }
        }
        Material.background: Material.color(Material.Orange)
        Material.foreground: "white"
        font.pointSize: 16
        width: implicitWidth + 20
        height: implicitHeight + 10
        icon.source: {
            if (App.globalVariables.isSteamVersion()) {
                return "qrc:/qml/ScreenPlayApp/assets/icons/icon_steam.svg";
            } else {
                return "qrc:/qml/ScreenPlayApp/assets/icons/icon_community.svg";
            }
        }
        icon.width: 18
        icon.height: 18
        onClicked: {
            if (App.globalVariables.isSteamVersion()) {
                App.util.setNavigation("Workshop");
            } else {
                Qt.openUrlExternally("https://forum.screen-play.app/");
            }
        }

        transform: [
            Shake {
                id: animShake
            },
            Grow {
                id: animGrow

                centerX: btnWorkshop.width * 0.5
                centerY: btnWorkshop.height * 0.5
                loops: -1
            }
        ]

        anchors {
            bottom: parent.bottom
            bottomMargin: -100
            horizontalCenter: parent.horizontalCenter
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

                ScriptAction {
                    script: {
                        animShake.start(2000, 1000, -1);
                    }
                }
            }
        }
    ]
}
