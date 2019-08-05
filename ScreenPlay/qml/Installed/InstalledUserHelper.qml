import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.2

import "../Common"

Item {
    id: installedUserHelper
    signal helperButtonPressed(var pos)
    anchors {
        fill: parent
    }

    state: "out"
    Component.onCompleted: state = "in"

    Image {
        id: imgBg
        source: "qrc:/assets/images/Intro.png"
        anchors.fill: parent
    }

    Item {
        height: parent.height
        anchors {
            top: parent.top
            topMargin: parent.height * .5 + 50
            right: parent.right
            left: parent.left
        }

        Image {
            id: imgShine
            source: "qrc:/assets/images/Intro_shine.png"
            height: 1753
            width: 1753
            anchors.centerIn: parent
            RotationAnimator {
                target: imgShine
                from: 0
                to: 360
                duration: 100000
                running: true
            }
        }
    }

    Text {
        id: txtHeadline2
        x: 20
        y: 80
        text: qsTr("Get free Widgets and Wallpaper via the Steam Workshop")
        font.family: "Roboto"

        wrapMode: Text.WordWrap
        color: "white"
        font.weight: Font.Thin
        font.pointSize: 32
        horizontalAlignment: Text.AlignHCenter
        anchors {
            right: parent.right
            left: parent.left
            top: parent.top
        }
    }

    Image {
        id: imgPC
        source: "qrc:/assets/images/Intro_PC.png"
        anchors {
            top: parent.top
            topMargin: 50
            horizontalCenter: parent.horizontalCenter
        }
        width: 500
        height: 500
        sourceSize: Qt.size(width, height)
    }

    Button {
        id: btnWorkshop
        text: qsTr("Browse Steam Workshop")
        Material.background: Material.Blue
        Material.foreground: "white"
        smooth: true
        font.pointSize: 18
        font.weight: Font.Thin
        transform: [
            Shake {
                id: animShake
            },
            Grow {
                id: animGrow
                centerX: btnWorkshop.width *.5
                centerY: btnWorkshop.height *.5
                loops: -1

            }
        ]
        anchors {
            bottom: parent.bottom
            bottomMargin: -100
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: helperButtonPressed(1)
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
                target: txtHeadline2
                opacity: 0
                anchors.topMargin: -300
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
                opacity: 1
            }
            PropertyChanges {
                target: imgPC
                opacity: 1
                anchors.topMargin: 150
            }

            PropertyChanges {
                target: txtHeadline2
                color: "#ffffff"
                opacity: 1
                anchors.topMargin: 100
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
                    duration: 500
                }

                PropertyAnimation {
                    targets: imgPC
                    property: "topMargin"
                    duration: 700
                    easing.type: Easing.OutBack
                }
            }

            SequentialAnimation {

                PauseAnimation {
                    duration: 100
                }
                PropertyAnimation {
                    targets: imgPC
                    property: "opacity"
                    duration: 700
                    easing.type: Easing.InOutExpo
                }
            }

            SequentialAnimation {

                PauseAnimation {
                    duration: 200
                }
                PropertyAnimation {
                    targets: txtHeadline2
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
                    duration: 400
                    easing.type: Easing.OutBack
                }
                ScriptAction {
                    script: {
                        animShake.start(2000,1000,-1)
                    }
                }
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/

