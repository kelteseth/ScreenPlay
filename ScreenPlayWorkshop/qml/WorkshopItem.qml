import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Material
import ScreenPlayWorkshop

Item {
    id: root
    objectName: "workshopItem" + itemIndex

    property url imgUrl
    property url additionalPreviewUrl
    property string name
    property var publishedFileID: 0
    property int itemIndex
    property int itemsPerRow: 4
    property int subscriptionCount
    property bool isDownloading: false
    property bool isScrolling: false
    property bool isInitialLoad: true
    property SteamWorkshop steamWorkshop

    readonly property int rowIndex: Math.floor(root.itemIndex / root.itemsPerRow)
    readonly property int columnIndex: root.itemIndex % root.itemsPerRow
    readonly property bool enableAnimations: !root.isScrolling

    signal clicked(var publishedFileID, url imgUrl)

    width: 320
    height: 180

    Component.onCompleted: {
        root.isInitialLoad = root.itemIndex < 20
        showAnimation.start()
    }

    Item {
        id: animatedContainer
        anchors.fill: parent
        opacity: 0

        Rectangle {
            id: itemWrapper
            color: Material.backgroundColor

            anchors {
                fill: parent
                margins: 5
            }

            Image {
                id: primaryImage
                anchors.fill: parent
                asynchronous: true
                cache: true
                sourceSize: Qt.size(320, 180)
                fillMode: Image.PreserveAspectCrop
                smooth: false
                source: root.imgUrl

                onStatusChanged: {
                    if (status === Image.Error) {
                        source = "qrc:/qt/qml/ScreenPlayWorkshop/assets/images/missingPreview.png"
                    }
                }
            }

            AnimatedImage {
                id: animatedImage
                anchors.fill: parent
                asynchronous: true
                playing: animatedImage.enabled
                sourceSize: Qt.size(320, 180)
                fillMode: Image.PreserveAspectCrop
                source: root.additionalPreviewUrl
                opacity: animatedImage.enabled ? 1 : 0
                enabled: !root.isScrolling && hoverArea.hovered && root.additionalPreviewUrl !== ""

                OpacityAnimator {
                    running: animatedImage.enabled
                    to: animatedImage.enabled ? 1 : 0
                    duration: 400
                    easing.type: Easing.OutQuart
                }
            }

            Rectangle {
                id: shadow

                height: 80
                opacity: 0

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                }

                gradient: Gradient {
                    GradientStop {
                        position: 1
                        color: "#CC000000"
                    }

                    GradientStop {
                        position: 0
                        color: "#00000000"
                    }
                }
            }

            Text {
                id: txtTitle

                text: root.name
                opacity: 0
                height: 30
                width: 180
                verticalAlignment: Text.AlignVCenter
                color: "white"
                font.pointSize: 14
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 20
                    left: parent.left
                    leftMargin: 20
                    bottomMargin: -50
                }
            }

            Item {
                id: openInWorkshop

                height: 20
                width: 20
                z: 99
                opacity: 0

                anchors {
                    margins: 10
                    top: parent.top
                    right: parent.right
                }

                Image {
                    source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
                    sourceSize: Qt.size(parent.width, parent.height)
                    fillMode: Image.PreserveAspectFit
                }
            }

            MultiEffect {
                id: effBlur
                anchors.fill: parent
                source: primaryImage
                blurEnabled: true
                blurMax: 64
                blur: 0
            }
        }

        MouseArea {
            anchors.fill: itemWrapper
            enabled: !root.isScrolling
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: function (mouse) {
                if (mouse.button === Qt.LeftButton) {
                    root.clicked(root.publishedFileID, root.imgUrl)
                }
            }
        }

        MouseArea {
            height: 20
            width: 20
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                Qt.openUrlExternally("steam://url/CommunityFilePage/" + root.publishedFileID)
            }

            anchors {
                margins: 10
                top: itemWrapper.top
                right: itemWrapper.right
            }
        }

        HoverHandler {
            id: hoverArea
            target: parent
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            cursorShape: root.isScrolling ? Qt.ArrowCursor : Qt.PointingHandCursor
        }
    }

    states: [
        State {
            name: "hover"
            when: hoverArea.hovered && root.enableAnimations && !root.isDownloading

            PropertyChanges {
                animatedContainer.scale: 1.05
            }

            PropertyChanges {
                openInWorkshop.opacity: 0.75
            }

            PropertyChanges {
                txtTitle.opacity: 1
                txtTitle.anchors.bottomMargin: 20
            }

            PropertyChanges {
                shadow.opacity: 1
            }
        },
        State {
            name: "downloading"
            when: root.isDownloading

            PropertyChanges {
                openInWorkshop.opacity: 0
            }

            PropertyChanges {
                txtTitle.opacity: 0
            }

            PropertyChanges {
                shadow.opacity: 0
            }

            PropertyChanges {
                effBlur.blur: 1.0
            }
        },
        State {
            name: "installed"

            PropertyChanges {
                txtTitle.opacity: 0
            }

            PropertyChanges {
                shadow.opacity: 0
            }

            PropertyChanges {
                effBlur.blur: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            enabled: root.enableAnimations

            ParallelAnimation {
                ScaleAnimator {
                    target: animatedContainer
                    duration: 300
                    easing.type: Easing.OutQuart
                }

                OpacityAnimator {
                    target: openInWorkshop
                    duration: 200
                    easing.type: Easing.OutQuart
                }

                OpacityAnimator {
                    target: txtTitle
                    duration: 200
                    easing.type: Easing.OutQuart
                }

                OpacityAnimator {
                    target: shadow
                    duration: 200
                    easing.type: Easing.OutQuart
                }

                NumberAnimation {
                    target: txtTitle
                    property: "anchors.bottomMargin"
                    duration: 200
                    easing.type: Easing.OutQuart
                }
            }
        },
        Transition {
            to: "downloading"
            reversible: true

            SequentialAnimation {
                NumberAnimation {
                    target: effBlur
                    property: "blur"
                    duration: 500
                    easing.type: Easing.OutQuart
                }
            }
        }
    ]

    SequentialAnimation {
        id: showAnimation
        running: false

        PauseAnimation {
            duration: {
                if (root.isInitialLoad) {
                    return Math.max(0, (root.rowIndex * 100) + (root.columnIndex * 50))
                } else {
                    return Math.max(0, root.columnIndex * 100)
                }
            }
        }

        ParallelAnimation {
            OpacityAnimator {
                target: animatedContainer
                from: 0
                to: 1
                duration: 600
                easing.type: Easing.OutCirc
            }

            YAnimator {
                target: animatedContainer
                from: 80
                to: 0
                duration: 500
                easing.type: Easing.OutCirc
            }

            ScaleAnimator {
                target: animatedContainer
                from: 0.3
                to: 1
                duration: 250
                easing.type: Easing.OutCirc
            }

            RotationAnimator {
                target: animatedContainer
                from: -5
                to: 0
                duration: 400
                easing.type: Easing.OutBack
            }
        }
    }
}
