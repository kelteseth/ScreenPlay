import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4

Item {
    id: installedUserHelper

    signal helperButtonPressed(var pos)

    state: "invisible"
    anchors {
        fill: parent
    }
    Component.onCompleted: {
        timerInit.start()
    }

    Timer {
        id: timerInit
        interval: 300
        onTriggered: installedUserHelper.state = "visible"
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    Text {
        id: txtIntro
        width: 600
        text: qsTr("Oh No! It looks like you dont have any wallpapers....")
        font.pixelSize: 28
        color: "#818181"
        renderType: Text.NativeRendering
        font.family: font_Roboto_Regular.name
        anchors {
            top: parent.top
            topMargin: 80
            horizontalCenter: parent.horizontalCenter
        }
    }

    Item {
        id: handWrapper
        width: 500
        height: 500
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.bottom
        }

        Item {
            id: handLeftWrapper
            height: 500
            width: 150
            anchors {
                bottom: parent.bottom
                right: parent.right
            }
            Image {
                id: imgHandLeft
                anchors.fill: parent
                height: 500
                width: 150
                sourceSize.height: 500
                sourceSize.width: 150
                source: "qrc:/assets/images/hand_give.png"
            }
            Item {
                width: 140
                anchors.topMargin: 90
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                Rectangle {
                    color: "#21A4D0"
                    z: 3
                    width: 140
                    height: 40
                    radius: 3
                    Text {
                        id: name2
                        anchors.centerIn: parent
                        text: qsTr("Create Your Own")
                        color: "white"
                        renderType: Text.NativeRendering
                        font.pixelSize: 14
                        font.family: font_Roboto_Regular.name
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            installedUserHelper.state = "invisible"
                            timerLeft.start()
                        }
                        Timer {
                            id: timerLeft
                            interval: 800
                            onTriggered: {
                                helperButtonPressed(0)
                            }
                        }
                    }
                }

                Rectangle {
                    color: "#0E79C2"
                    z: 1
                    width: 140
                    height: 44
                    radius: 3
                }
            }
        }

        Item {
            id: handLeRightWrapper
            height: 500

            width: 150
            anchors {
                bottom: parent.bottom
                left: parent.left
            }
            Image {
                id: imgHandRight
                anchors.fill: parent
                height: 500
                width: 150
                sourceSize.height: 500
                sourceSize.width: 150
                mirror: true
                source: "qrc:/assets/images/hand_give.png"
            }
            Item {
                width: 140
                anchors.topMargin: 90
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                Rectangle {
                    color: "#FF5A2E"
                    z: 3
                    width: 140
                    height: 40
                    radius: 3
                    Text {
                        id: name
                        anchors.centerIn: parent
                        text: qsTr("Browse Workshop")
                        color: "white"
                        renderType: Text.NativeRendering
                        font.pixelSize: 14
                        font.family: font_Roboto_Regular.name
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            installedUserHelper.state = "invisible"
                            timerRight.start()
                        }
                        Timer {
                            id: timerRight
                            interval: 800
                            onTriggered: {
                                helperButtonPressed(1)
                            }
                        }
                    }
                }

                Rectangle {
                    color: "#E92716"
                    z: 1
                    width: 140
                    height: 44
                    radius: 3
                }
            }
        }
    }

    Text {
        id: txtRightHand

        font.pixelSize: 24
        color: "#818181"
        renderType: Text.NativeRendering
        font.family: font_Roboto_Regular.name

        anchors {
            left: handWrapper.right
            leftMargin: 30
            top: handWrapper.top
            topMargin: 50
            right: parent.right
            rightMargin: 50
        }

        wrapMode: Text.WordWrap
        text: qsTr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. ")
    }

    Text {
        id: txtLeftHand
        font.pixelSize: 23
        color: "#818181"
        renderType: Text.NativeRendering
        font.family: font_Roboto_Regular.name
        anchors {
            right: handWrapper.left
            rightMargin: 30
            top: handWrapper.top
            topMargin: 50
            left: parent.left
            leftMargin: 50
        }
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignRight
        text: qsTr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. ")
    }

    states: [
        State {
            name: "invisible"

            PropertyChanges {
                target: handWrapper
                anchors.topMargin: 00
                opacity: 0
            }
            PropertyChanges {
                target: txtIntro
                opacity: 0
                anchors.topMargin: -50
            }

            PropertyChanges {
                target: txtLeftHand
                opacity: 0
            }
            PropertyChanges {
                target: txtRightHand
                opacity: 0
            }
        },
        State {
            name: "visible"
            PropertyChanges {
                target: handWrapper
                anchors.topMargin: -500
                opacity: 1
            }
            PropertyChanges {
                target: txtIntro
                opacity: 1
                anchors.topMargin: 80
            }
            PropertyChanges {
                target: txtLeftHand
                opacity: 1
            }
            PropertyChanges {
                target: txtRightHand
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: "invisible"
            to: "visible"
            SequentialAnimation {
                NumberAnimation {
                    target: txtIntro
                    properties: "opacity,anchors.topMargin"
                    duration: 500
                    easing.type: Easing.OutCubic
                }
                PauseAnimation {
                    duration: 400
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: handWrapper
                        property: "anchors.topMargin"
                        duration: 600
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: handWrapper
                        property: "opacity"
                        duration: 300
                        easing.type: Easing.InQuart
                    }
                }
                PauseAnimation {
                    duration: 400
                }
                NumberAnimation {
                    targets: [txtLeftHand, txtRightHand]
                    property: "opacity"
                    duration: 300
                    easing.type: Easing.InQuart
                }
            }
        },
        Transition {
            from: "visible"
            to: "invisible"
            ParallelAnimation {
                NumberAnimation {
                    targets: [txtLeftHand, txtRightHand]
                    property: "opacity"
                    duration: 300
                    easing.type: Easing.InQuart
                }

                NumberAnimation {
                    target: txtIntro
                    properties: "opacity,anchors.topMargin"
                    duration: 500
                    easing.type: Easing.OutCubic
                }

                NumberAnimation {
                    target: handWrapper
                    property: "anchors.topMargin"
                    duration: 600
                    easing.type: Easing.Linear
                }
                NumberAnimation {
                    target: handWrapper
                    property: "opacity"
                    duration: 300
                    easing.type: Easing.InQuart
                }
            }
        }
    ]
}
