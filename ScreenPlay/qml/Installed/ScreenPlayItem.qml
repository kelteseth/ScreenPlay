import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4

Item {
    id: screenPlayItem
    width: 320
    height: 180
    state: "invisible"
    opacity: 0

    property string customTitle: "name here"
    property url absoluteStoragePath
    property string type
    property bool hasMenuOpen: false
    property int workshopID: 0
    property int itemIndex
    property string screenId: ""
    signal itemClicked(var screenId, var type)

    onTypeChanged: {
        if (type === "widget") {
            icnType.source = "qrc:/assets/icons/icon_widgets.svg"
        } else if (type === "qmlScene") {
            icnType.source = "qrc:/assets/icons/icon_code.svg"
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
                                 screenPlayItem.absoluteStoragePath
                                 + "/" + screenPreview)
                sourceImageGIF: {
                    if (screenPreviewGIF === undefined) {
                        return ""
                    } else {
                        return Qt.resolvedUrl(
                                    screenPlayItem.absoluteStoragePath
                                    + "/" + screenPreviewGIF)

                    }
                }
            }

            Image {
                id: icnType
                width: 20
                height: 20
                opacity: 0
                source: "qrc:/assets/icons/icon_movie.svg"
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
                    font.family: "Segoe UI, Roboto"
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
                cursorShape: Qt.PointingHandCursor
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onEntered: {
                    if (!hasMenuOpen) {
                        screenPlayItem.state = "hover"
                        screenPlayItemImage.enter()
                    }
                }
                onExited: {
                    if (!hasMenuOpen) {
                        screenPlayItem.state = "visible"
                        screenPlayItemImage.exit()
                    }
                }

                onClicked: {
                    if (mouse.button === Qt.LeftButton) {
                        itemClicked(screenId, type)
                    } else if (mouse.button === Qt.RightButton) {
                        if (workshopID != 0) {
                            miWorkshop.enabled = true
                        }

                        contextMenu.popup()
                        hasMenuOpen = true
                    }
                }
            }
        }
        Menu {
            id: contextMenu
            onClosed: hasMenuOpen = false
            MenuItem {
                text: qsTr("Open containing folder")
                onClicked: {
                    utility.openFolderInExplorer(absoluteStoragePath)
                }
            }
            MenuItem {
                id: miWorkshop
                text: qsTr("Open workshop Page")
                enabled: false

                onClicked: {
                    Qt.openUrlExternally(
                                "steam://url/CommunityFilePage/" + workshopID)
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

            //            PropertyAnimation {
            //                target: screenPlayItemWrapper
            //                properties: "y"
            //                duration: 300
            //                easing.type: Easing.OutQuart
            //            }
            //            OpacityAnimator {
            //                target: screenPlayItemWrapper
            //                duration: 500
            //                easing.type: Easing.OutQuart
            //            }
            //            PropertyAnimation {
            //                target: effect
            //                property: "opacity"
            //                duration: 500
            //                easing.type: Easing.OutQuart
            //            }
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
