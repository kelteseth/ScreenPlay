import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4

Item {
    id: installedUserHelper
    state: "invisible"
    anchors {
        fill: parent
    }
    Component.onCompleted: {
        installedUserHelper.state = "visible"
    }

    Item {
        id: handWrapper
        width: 350
        height: 500
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
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
            Button {
                id: button
                text: qsTr("asasas")
                anchors.topMargin: 120
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {

                }

                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 25
                    radius: 2
                    color:"#21A4D0"
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: button.hovered ? "#21B6D0" : "#21A4D0"
                        }
                        GradientStop {
                            position: 1
                            color: button.hovered ? "#21B6D0" : "#21A4D0"
                        }
                    }
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
            Button {
                text: qsTr("asasas")
                anchors.topMargin: 120
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    states: [
        State {
            name: "invisible"
            AnchorChanges {
                target: handWrapper
                anchors.top: installedUserHelper.bottom
                anchors.bottom: undefined
            }
        },
        State {
            name: "visible"
            AnchorChanges {
                target: handWrapper
                anchors.top: undefined
                anchors.bottom: installedUserHelper.bottom
            }
        }
    ]

    transitions: Transition {
        AnchorAnimation {
            duration: 1200
            easing.type: Easing.InOutQuad

            onStopped: {
                print("stopped")
                imgHandRight.source = "qrc:/assets/icons/icon_hand_right.svg"
                imgHandRight.sourceSize.height = 500
                imgHandRight.sourceSize.width = 150
            }
        }
    }
}
