import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    id: screenPlayItem
    width: 320
    height: 180
    state: "invisible"
    Component.onCompleted: screenPlayItem.state = "visible"

    property string customTitle: "name here"
    property url absoluteStoragePath

    property real introTime: Math.random() * (1 - .5) + .5
    property string screenId: ""
    signal itemClicked(var screenId)

    RectangularGlow {
        id: effect
        anchors {
            top: parent.top
            topMargin: 3
        }

        height: parent.height
        width: parent.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Item {
        id: screenPlayItemWrapper
        anchors.centerIn: parent
        height: 180
        width: 320

        Image {
            id: mask
            source: "qrc:/assets/images/Window.svg"
            sourceSize: Qt.size(screenPlayItem.width, screenPlayItem.height)
            visible: false
            smooth: true
            fillMode: Image.PreserveAspectFit
            antialiasing: true
        }

        Item {
            id: itemWrapper
            visible: false
            anchors.fill: parent

            ScreenPlayItemImage {
                id: screenPlayItemImage
                anchors.fill: parent
                sourceImage: Qt.resolvedUrl(
                                 "file:///" + screenPlayItem.absoluteStoragePath
                                 + "/" + screenPreview)
            }

            Rectangle {
                color: "#AAffffff"
                height: 30
                visible: false
                anchors {
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }

                Text {
                    id: text1
                    height: 29
                    text: screenTitle
                    anchors {
                        right: parent.right
                        left: parent.left
                        top: parent.top
                        margins: 10
                    }
                    wrapMode: Text.WordWrap

                    color: "#2F2F2F"
                    font.pointSize: 9
                    renderType: Text.NativeRendering
                    font.family: font_Roboto_Regular.name

                    FontLoader {
                        id: font_Roboto_Regular
                        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
                    }
                }
            }
        }

        OpacityMask {
            anchors.fill: itemWrapper
            antialiasing: true
            source: itemWrapper
            maskSource: mask

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    screenPlayItem.state = "hover"
                }
                onExited: {
                    screenPlayItem.state = "visible"
                }

                onClicked: {
                    itemClicked(screenId)
                }
            }
        }
    }

    states: [
        State {
            name: "invisible"

            PropertyChanges {
                target: screenPlayItemWrapper
                y: -10
                opacity: 0
            }
            PropertyChanges {
                target: effect
                opacity: 0
            }
        },
        State {
            name: "visible"
            PropertyChanges {
                target: effect
                opacity: 0.4
            }
            PropertyChanges {
                target: screenPlayItemWrapper
                y: 0
                opacity: 1
            }
            PropertyChanges {
                target: screenPlayItem
                width: 320
                height:180

            }
        },
        State {
            name: "description"
        },
        State {
            name: "hover"
            PropertyChanges {
                target: screenPlayItemWrapper
                width: 330
                height:190
            }
            PropertyChanges {
                target: effect
                opacity: 0.4
            }
            PropertyChanges {
                target: screenPlayItemWrapper
                y: 0
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "invisible"
            to: "visible"

            PropertyAnimation {
                target: screenPlayItemWrapper
                properties: "y"
                duration: 300 * introTime //* (number *.1)
                easing.type: Easing.InOutQuad
            }
            OpacityAnimator {
                target: screenPlayItemWrapper
                duration: 50 * introTime //* (number *.1)
                easing.type: Easing.InOutQuad
            }
            PropertyAnimation {
                target: effect
                property: "opacity"
                duration: 500
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "visible"
            to: "hover"
            reversible: true

            PropertyAnimation {
                target: screenPlayItemWrapper
                properties: "width,height"
                duration: 100
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
