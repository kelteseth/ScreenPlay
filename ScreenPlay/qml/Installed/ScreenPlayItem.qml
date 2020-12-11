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
        case InstalledType.GifWallpaper:
        case InstalledType.WebsiteWallpaper:
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

            5 * itemIndexMax * Math.random()
        }

        running: true
        onTriggered: showAnim.start()
    }

    ParallelAnimation {
        id: showAnim
        running: false
        OpacityAnimator {
            target: screenPlayItemWrapper
            from: 0
            to: 1
            duration: 800
            easing.type: Easing.OutCirc
        }
        YAnimator {
            target: screenPlayItemWrapper
            from: 80
            to: 0
            duration: 500
            easing.type: Easing.OutCirc
        }
        ScaleAnimator {
            target: screenPlayItemWrapper
            from: .5
            to: 1
            duration: 200
            easing.type: Easing.OutCirc
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
        width: 320
        height: 180
        opacity: 0

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

    transitions: [
        Transition {
            from: ""
            to: "hover"

            ScaleAnimator {
                target: screenPlayItemWrapper
                duration: 80
                from: 1
                to: 1.05
            }
            ScaleAnimator {
                target: effect
                duration: 80
                from: 1
                to: 1.05
            }
            OpacityAnimator {
                target: icnType
                duration: 80
                from: 0.5
                to: 1
            }
            OpacityAnimator {
                target: effect
                duration: 80
                from: 0.6
                to: 1
            }
        },
        Transition {
            from: "hover"
            to: ""

            ScaleAnimator {
                target: screenPlayItemWrapper
                duration: 80
                from: 1.05
                to: 1
            }
            ScaleAnimator {
                target: effect
                duration: 80
                from: 1.05
                to: 1
            }
            OpacityAnimator {
                target: icnType
                duration: 80
                from: 1
                to: 0.5
            }
            OpacityAnimator {
                target: effect
                duration: 80
                from: 1
                to: 0.5
            }
        }
    ]
}
