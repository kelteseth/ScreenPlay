import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Enums.InstalledType
import ScreenPlayUtil as Util

Item {
    id: root

    property string customTitle
    property string screenId
    property url absoluteStoragePath
    property int type: InstalledType.Unknown
    // Must be var to make it work wit 64bit ints
    property var publishedFileID: 0
    property int itemIndex
    property bool isScrolling: false
    property bool isNew: false
    property bool containsAudio: false

    signal openContextMenu(point position)

    width: 320
    height: 180
    onTypeChanged: {
        if (Util.JSUtil.isWidget(type)) {
            icnType.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_widgets.svg";
            return;
        }
        if (Util.JSUtil.isScene(type)) {
            icnType.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_code.svg";
            return;
        }
        if (Util.JSUtil.isVideo(type)) {
            icnType.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_movie.svg";
            return;
        }
    }

    Timer {
        running: true
        onTriggered: showAnim.start()
        interval: {
            var itemIndexMax = itemIndex;
            if (itemIndex > 30)
                itemIndexMax = 3;
            5 * itemIndexMax * Math.random();
        }
    }

    SequentialAnimation {
        id: showAnim

        running: false

        ParallelAnimation {
            OpacityAnimator {
                target: screenPlayItemWrapper
                from: 0
                to: 1
                duration: 600
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
                from: 0.5
                to: 1
                duration: 200
                easing.type: Easing.OutCirc
            }
        }

        OpacityAnimator {
            target: effect
            from: 0
            to: 0.4
            duration: 800
            easing.type: Easing.OutCirc
        }
    }

    RectangularGlow {
        id: effect

        height: parent.height
        width: parent.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0
        cornerRadius: 15

        anchors {
            top: parent.top
            topMargin: 3
        }
    }

    Item {
        id: screenPlayItemWrapper

        width: 320
        height: 180
        opacity: 0

        Image {
            id: mask

            source: "qrc:/qml/ScreenPlayApp/assets/images/Window.svg"
            sourceSize: Qt.size(root.width, root.height)
            visible: false
            smooth: true
            fillMode: Image.PreserveAspectFit
        }

        Item {
            id: itemWrapper

            visible: false
            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                color: Material.backgroundColor
            }

            Text {
                text: root.customTitle
                font.family: App.settings.font
                font.pointSize: 16
                visible: !screenPlayItemImage.visible
                color: Material.primaryTextColor
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
            }

            ScreenPlayItemImage {
                id: screenPlayItemImage

                anchors.fill: parent
                enabled: visible
                visible: m_preview !== "" || m_previewGIF !== ""
                sourceImage: m_preview
                sourceImageGIF: m_previewGIF
                type: root.type
                absoluteStoragePath: m_absoluteStoragePath
            }

            Image {
                id: icnAudio

                width: 20
                height: 20
                opacity: 0.25
                visible: root.containsAudio
                source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_contains_audio.svg"
                sourceSize: Qt.size(20, 20)

                anchors {
                    top: parent.top
                    left: icnType.right
                    margins: 10
                }
            }

            Image {
                id: icnType

                width: 20
                height: 20
                opacity: 0.25
                source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_movie.svg"
                sourceSize: Qt.size(20, 20)

                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 10
                }
            }

            Rectangle {

                width: 120
                height: 20
                anchors {
                    top: parent.top
                    right: parent.right
                    rightMargin: -60
                    topMargin: -20
                }
                color: Material.color(Material.Orange)
                transform: Rotation {
                    angle: 45
                }
                visible: root.isNew

                Text {
                    font.family: App.settings.font
                    font.pointSize: 9
                    renderType: Text.QtRendering
                    color: "white"
                    text: qsTr("NEW")
                    smooth: true
                    antialiasing: true
                    visible: root.isNew
                    anchors.centerIn: parent
                }
            }
        }

        OpacityMask {
            anchors.fill: itemWrapper
            source: itemWrapper
            maskSource: mask

            MouseArea {
                anchors.fill: parent
                hoverEnabled: !root.isScrolling && !showAnim.running
                cursorShape: Qt.PointingHandCursor
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onEntered: {
                    root.state = "hover";
                    screenPlayItemImage.state = "hover";
                    screenPlayItemImage.enter();
                }
                onExited: {
                    root.state = "";
                    screenPlayItemImage.state = "loaded";
                    screenPlayItemImage.exit();
                }
                onClicked: function (mouse) {
                    if (mouse.button === Qt.LeftButton)
                        App.util.setSidebarItem(root.screenId, root.type);
                    else if (mouse.button === Qt.RightButton)
                        root.openContextMenu(Qt.point(mouseX, mouseY));
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
                from: 0.25
                to: 0.8
            }

            OpacityAnimator {
                target: icnAudio
                duration: 80
                from: 0.25
                to: 0.8
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
                from: 0.8
                to: 0.25
            }

            OpacityAnimator {
                target: icnAudio
                duration: 80
                from: 0.8
                to: 0.25
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
