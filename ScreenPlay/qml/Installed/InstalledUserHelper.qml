import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.2

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

    Item {
        id: txtWrapper
        width: 600
        height: 150
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 100
        }

        Text {
            id: txtHeadline
            text: qsTr("Make your desktop more:")
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
            }
            font.family: "Roboto"
            color: "white"
            font.weight: Font.Normal
            
        }

        Text {
            id: txtHeadline2
            text: qsTr("DISTINCT")
            font.family: "Roboto Mono Thin"
            font.pointSize: 75
            color: "white"
            font.weight: Font.Normal
            
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
            }
        }
    }

    Image {
        id: imgPC
        source: "qrc:/assets/images/Intro_PC.png"
        anchors {
            top: txtWrapper.bottom
            topMargin: -50
            horizontalCenter: parent.horizontalCenter
        }
        width: 500
        height: 500
        sourceSize: Qt.size(width, height)
    }

    Button {
        id: btnWorkshop
        text: qsTr("Get free Widgets and Wallpaper")
        Material.background: Material.Orange
        Material.foreground: "white"
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
                anchors.topMargin: -300
            }
            PropertyChanges {
                target: txtHeadline
                opacity: 0
                anchors.topMargin: -100
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
                anchors.topMargin: -100
            }

            PropertyChanges {
                target: txtHeadline2
                color: "#ffffff"
            }
            PropertyChanges {
                target: txtHeadline
                font.pointSize: 16
                font.wordSpacing: -2
                opacity: 1
                anchors.topMargin: 0
            }
            PropertyChanges {
                target: txtHeadline2
                opacity: 1
                anchors.topMargin: 30
            }

            PropertyChanges {
                target: btnWorkshop
                anchors.bottomMargin: 100
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true

            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation {
                        targets: imgBg
                        property: "opacity"
                        duration: 200
                    }
                    PropertyAnimation {
                        targets: imgShine
                        property: "opacity"
                        duration: 400
                    }
                }
                ParallelAnimation {
                    PropertyAnimation {
                        targets: imgPC
                        property: "topMargin"
                        duration: 800
                        easing.type: Easing.OutBack
                    }
                    PropertyAnimation {
                        targets: imgPC
                        property: "opacity"
                        duration: 800
                        easing.type: Easing.InOutExpo
                    }
                }

                PauseAnimation {
                    duration: 200
                }

                ParallelAnimation {
                    PropertyAnimation {
                        targets: txtHeadline
                        properties: "topMargin, opacity"
                        duration: 800
                        easing.type: Easing.InOutExpo
                    }
                    PropertyAnimation {
                        targets: txtHeadline2
                        properties: "topMargin, opacity"
                        duration: 1000
                        easing.type: Easing.InOutExpo
                    }
                    PropertyAnimation {
                        targets: btnWorkshop
                        properties: "bottomMargin"
                        duration: 300
                        easing.type: Easing.OutBack
                    }
                }
            }
        }
    ]
}
