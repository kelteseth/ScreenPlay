import QtQuick 2.9
import QtGraphicalEffects 1.0

Item {
    id: screenPlayItem
    width: 320
    height: 180
    state: "invisible"
    property string customTitle: "name here"
    property url absoluteStoragePath
    property string type
    onTypeChanged: {
        if (type === "widget") {
            icnType.source = "qrc:/assets/icons/icon_widgets.svg"
        }
    }

    property real introTime: Math.random() * (1 - .5) + .5
    property string screenId: ""

    signal itemClicked(var screenId, var type)

    Component.onCompleted: {
        screenPlayItem.state = "visible"
    }

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

            Image {
                id: icnType
                width: 15
                height: 15
                opacity: 0
                source: "qrc:/assets/icons/icon_movie.svg"
                sourceSize: Qt.size(15, 15)
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 10
                }
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
                    font.family: "Roboto"
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
                    itemClicked(screenId, type)
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
                height: 180
            }
            PropertyChanges {
                target: icnType
                opacity: 0
            }
            PropertyChanges {
                target: screenPlayItemWrapper
                y: 0
                opacity: 1
            }
        },
        State {
            name: "description"
        },
        State {
            name: "hover"
            PropertyChanges {
                target: screenPlayItemWrapper
                width: 325
                height: 185
            }
            PropertyChanges {
                target: effect
                opacity: 0.6
                width: 325
                height: 185
            }
            PropertyChanges {
                target: screenPlayItemWrapper
                y: 0
                opacity: 1
            }
            PropertyChanges {
                target: icnType
                opacity: .5
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
                duration: 80
            }
            PropertyAnimation {
                target: effect
                properties: "width,height,opacity"
                duration: 80
            }
            PropertyAnimation {
                target: icnType
                property: "opacity"
                duration: 80
            }
        }
    ]
}