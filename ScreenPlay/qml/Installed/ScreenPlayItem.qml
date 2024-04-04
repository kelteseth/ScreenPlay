import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlayUtil as Util

Item {
    id: root

    property string customTitle
    property string screenId
    property url absoluteStoragePath
    property int type: Util.ContentTypes.InstalledType.Unknown
    // Must be var to make it work wit 64bit ints
    property var publishedFileID: 0
    property int itemIndex
    property bool isScrolling: false
    property bool isNew: false
    property bool containsAudio: false
    property int version: App.globalVariables.version
    property bool hasLicense: {
        if ((root.version === GlobalVariables.OpenSourceStandalone
             || root.version === GlobalVariables.OpenSourceSteam)
                && root.type === Util.ContentTypes.InstalledType.GodotWallpaper) {
            return false
        }
        return true
    }

    signal clicked(var screenId, var type)
    signal openContextMenu(var point)
    signal openOpenLicensePopup()

    width: 320
    height: 180
    onTypeChanged: {
        if (App.util.isWidget(type)) {
            icnType.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_widgets.svg"
            return
        }
        if (App.util.isScene(type)) {
            icnType.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_code.svg"
            return
        }
        if (App.util.isVideo(type)) {
            icnType.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_movie.svg"
            return
        }
    }

    Timer {
        running: true
        onTriggered: showAnim.start()
        interval: {
            var itemIndexMax = itemIndex
            if (itemIndex > 30)
                itemIndexMax = 3
            5 * itemIndexMax * Math.random()
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
                visible: !screenPlayItemImage.visible && root.hasLicense
                color: Material.primaryTextColor
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
            }

            ScreenPlayItemImage {
                id: screenPlayItemImage
                opacity: root.hasLicense ? 1 : 0.3
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

            ToolButton {
                enabled: false
                visible: !root.hasLicense
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/font-awsome/lock-solid.svg"
                icon.height: 14
                icon.width: 11
                icon.color: "gold"
                opacity: .5
                anchors {
                    top: parent.top
                    right: parent.right
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
                    if (!root.hasLicense)
                        return
                    root.state = "hover"
                    screenPlayItemImage.state = "hover"
                    screenPlayItemImage.enter()
                }
                onExited: {
                    root.state = ""
                    screenPlayItemImage.state = "loaded"
                    screenPlayItemImage.exit()
                }
                onClicked: function (mouse) {
                    if (!root.hasLicense) {
                        root.openOpenLicensePopup()
                        return
                    }

                    if (mouse.button === Qt.LeftButton)
                        root.clicked(root.screenId, root.type)
                    else if (mouse.button === Qt.RightButton)
                        root.openContextMenu(Qt.point(mouseX, mouseY))
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
                duration: 2000
                from: 1
                to: 1.05
                easing.type: Easing.OutQuart
            }

            ScaleAnimator {
                target: effect
                duration: 2000
                from: 1
                to: 1.05
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnType
                duration: 2000
                from: 0.25
                to: 0.8
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnAudio
                duration: 2000
                from: 0.25
                to: 0.8
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: effect
                duration: 2000
                from: 0.6
                to: 1
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "hover"
            to: ""

            ScaleAnimator {
                target: screenPlayItemWrapper
                duration: 2000
                from: 1.05
                to: 1
                easing.type: Easing.OutQuart
            }

            ScaleAnimator {
                target: effect
                duration: 2000
                from: 1.05
                to: 1
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnType
                duration: 2000
                from: 0.8
                to: 0.25
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnAudio
                duration: 2000
                from: 0.8
                to: 0.25
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: effect
                duration: 2000
                from: 1
                to: 0.5
                easing.type: Easing.OutQuart
            }
        }
    ]
}
