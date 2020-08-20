import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4

Item {
    id: screenPlayItem
    width: 320
    height: 180
    property alias checkBox: checkBox
    state: "invisible"
    opacity: 0

    property string preview: screenPreview
    property bool isSelected: false
    property string customTitle: "name here"
    property string absoluteStoragePath
    property string type
    property bool hasMenuOpen: false
    property int workshopID: 0
    property int itemIndex
    property string screenId: ""
    signal itemClicked(var screenId, var type, var isActive)

    onTypeChanged: {
        if (type === "widget") {
            icnType.source = "icons/icon_widgets.svg"
        } else if (type === "qmlScene") {
            icnType.source = "icons/icon_code.svg"
        }
    }

    Component.onCompleted: {
        screenPlayItem.state = "visible"
    }

    Timer {
        id: timerAnim
        interval: 40 * itemIndex * Math.random()
        running: true
        repeat: false
        onTriggered: showAnim.start()
    }

    transform: [
        Rotation {
            id: rt
            origin.x: width * .5
            origin.y: height * .5
            axis {
                x: -.5
                y: 0
                z: 0
            }
            angle: 0
        },
        Translate {
            id: tr
        },
        Scale {
            id: sc
            origin.x: width * .5
            origin.y: height * .5
        }
    ]
    ParallelAnimation {
        id: showAnim
        running: false
        RotationAnimation {
            target: rt
            from: 90
            to: 0
            duration: 500
            easing.type: Easing.OutQuint
            property: "angle"
        }
        PropertyAnimation {
            target: screenPlayItem
            from: 0
            to: 1
            duration: 500
            easing.type: Easing.OutQuint
            property: "opacity"
        }
        PropertyAnimation {
            target: tr
            from: 80
            to: 0
            duration: 500
            easing.type: Easing.OutQuint
            property: "y"
        }
        PropertyAnimation {
            target: sc
            from: .8
            to: 1
            duration: 500
            easing.type: Easing.OutQuint
            properties: "xScale,yScale"
        }
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
            source: "qrc:/assets/img/window.svg"
            sourceSize: Qt.size(screenPlayItem.width, screenPlayItem.height)
            visible: false
            smooth: true
            fillMode: Image.PreserveAspectFit
        }

        Item {
            id: itemWrapper
            anchors.fill: parent
            visible: false

            ScreenPlayItemImage {
                id: screenPlayItemImage
                anchors.fill: parent
                sourceImage: Qt.resolvedUrl(
                                 screenPlayItem.absoluteStoragePath + "/" + screenPreview)


            }

            Image {
                id: icnType
                width: 20
                height: 20
                opacity: 0
                sourceSize: Qt.size(20, 20)
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
            }
        }

        OpacityMask {
            anchors.fill: itemWrapper
            source: itemWrapper
            maskSource: mask

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onEntered: {
                    if (!hasMenuOpen) {
                        screenPlayItem.state = "hover"
                    }
                }
                onExited: {
                    if (!hasMenuOpen) {
                        screenPlayItem.state = "visible"
                    }
                }

                onClicked: {
                    checkBox.toggle()
                    if (mouse.button === Qt.LeftButton) {
                        itemClicked(screenId, type, checkBox.checkState === Qt.Checked)
                    }
                }
            }
        }

        CheckBox {
            id: checkBox
            onCheckStateChanged: {
                if(checkState == Qt.Checked){
                    isSelected = true
                } else {
                    isSelected = false
                }
            }

            anchors {
                top: parent.top
                right: parent.right
                margins: 10
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
        },
        State {
            name: "selected"

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
        },
        Transition {
            from: "visible"
            to: "selected"
            reversible: true

            PropertyAnimation {
                target: icnType
                property: "opacity"
                duration: 80
            }
        }
    ]
}
