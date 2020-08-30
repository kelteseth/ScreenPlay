import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0

Item {
    id: root
    width: 320
    height: 180
    opacity: 0

    property string customTitle: "name here"
    property url absoluteStoragePath
    property var type
    property int workshopID: 0
    property int itemIndex
    property string screenId: ""

    signal openContextMenu(point position)

    onTypeChanged: {
        switch (type) {
        case InstalledType.Unknown:
            return
        case InstalledType.VideoWallpaper:
        case InstalledType.QMLWallpaper:
        case InstalledType.HTMLWallpaper:
        case InstalledType.GodotWallpaper:
            icnType.source = "qrc:/assets/icons/icon_widgets.svg"
            return
        case InstalledType.QMLWidget:
        case InstalledType.HTMLWidget:
            icnType.source = "qrc:/assets/icons/icon_movie.svg"
            return
        }
    }

    Timer {
        id: timerAnim
        interval: {
            var itemIndexMax = itemIndex
            if (itemIndex > 30)
                itemIndexMax = 3

            10 * itemIndexMax * Math.random()
        }

        running: true
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
            from: 60
            to: 0
            duration: 500
            easing.type: Easing.OutCirc
            property: "angle"
        }
        PropertyAnimation {
            target: root
            from: 0
            to: 1
            duration: 800
            easing.type: Easing.OutCirc
            property: "opacity"
        }
        PropertyAnimation {
            target: tr
            from: 80
            to: 0
            duration: 500
            easing.type: Easing.OutCirc
            property: "y"
        }
        PropertyAnimation {
            target: sc
            from: .5
            to: 1
            duration: 200
            easing.type: Easing.OutCirc
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
            sourceSize: Qt.size(root.width, root.height)
            visible: false
            smooth: true
            fillMode: Image.PreserveAspectFit
        }

        Item {
            id: itemWrapper
            visible: false
            anchors.fill: parent

            ScreenPlayItemImage {
                id: screenPlayItemImage
                anchors.fill: parent
                sourceImage: m_preview
                sourceImageGIF: m_previewGIF
                absoluteStoragePath: m_absoluteStoragePath
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
                    root.state = "hover"
                    screenPlayItemImage.state = "hover"
                    screenPlayItemImage.enter()
                }
                onExited: {
                    root.state = ""
                    screenPlayItemImage.state = "loaded"
                    screenPlayItemImage.exit()
                }

                onClicked: {
                    if (mouse.button === Qt.LeftButton) {
                        ScreenPlay.util.setSidebarItem(root.screenId, root.type)
                    } else if (mouse.button === Qt.RightButton) {
                        root.openContextMenu(Qt.point(mouseX, mouseY))
                    }
                }
            }
        }
    }

    states: [

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
            from: ""
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
