import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import ScreenPlay
import ScreenPlayCore as Util

Item {
    id: root
    // Required properties - using readonly for better performance where appropriate
    required property string title
    required property string folderName
    required property string absoluteStoragePath
    required property int type
    required property var publishedFileID  // var for 64-bit support
    required property bool isNew
    required property bool containsAudio
    required property string previewGIF
    required property string preview
    required property int index
    required property int itemsPerRow
    readonly property int rowIndex: Math.floor(root.index / root.itemsPerRow)
    readonly property int columnIndex: root.index % itemsPerRow
    property bool isScrolling: false

    // Using readonly properties avoids creating bindings when values are static
    readonly property bool hasLicense: {
        if (App.globalVariables.isBasicVersion()) {
            return false;
        }
        return true;
    }

    // Pre-compute image sources to avoid repeated string concatenation
    readonly property string primaryImageSource: root.preview === "" ?
        "qrc:/qt/qml/ScreenPlay/assets/images/missingPreview.png" :
        Qt.resolvedUrl(root.absoluteStoragePath + "/" + root.preview)

    readonly property string gifImageSource: root.previewGIF === "" ?
        "" : Qt.resolvedUrl(root.absoluteStoragePath + "/" + root.previewGIF)

    // Compute type icon once to avoid repeated checks
    readonly property string typeIconSource: {
        if (App.util.isWidget(type)) {
            return "qrc:/qt/qml/ScreenPlay/assets/icons/icon_widgets.svg";
        }
        if (App.util.isScene(type)) {
            return "qrc:/qt/qml/ScreenPlay/assets/icons/icon_code.svg";
        }
        return "qrc:/qt/qml/ScreenPlay/assets/icons/icon_movie.svg";
    }

    // Performance flags
    readonly property bool enableAnimations: !root.isScrolling

    signal clicked(var folderName, var type)
    signal openContextMenu(var point)
    signal openOpenLicensePopup


    property bool isInitialLoad: true


    Component.onCompleted: {
        root.isInitialLoad = root.index < 20;
        showAnimation.start();
    }

    width: 320
    height: 180

    Item {
        id: animatedContainer
        anchors.fill: parent
        clip: root.isNew // Only needed for NEW orange banner
        opacity: 0  // Start invisible for entrance animation

        Rectangle {
            id: itemWrapper
            anchors.fill: parent
            color: Material.backgroundColor

            Image {
                id: primaryImage
                anchors.fill: parent
                asynchronous: true
                cache: true
                sourceSize: Qt.size(320, 180)
                fillMode: Image.PreserveAspectCrop
                smooth: false

                source: root.primaryImageSource
                opacity: root.hasLicense ? 1 : 0.3
                visible: root.preview !== ""

                // Handle loading states efficiently
                onStatusChanged: {
                    if (status === Image.Error) {
                        source = "qrc:/qt/qml/ScreenPlay/assets/images/missingPreview.png";
                    }
                }
            }

            AnimatedImage {
                id: gifImage
                anchors.fill: parent
                asynchronous: true
                playing: gifImage.enabled
                sourceSize: Qt.size(320, 180)
                fillMode: Image.PreserveAspectCrop
                source: root.gifImageSource
                opacity: gifImage.enabled ? 1 : 0
                enabled: !root.isScrolling && hoverArea.hovered && root.gifImageSource !== ""

                OpacityAnimator {
                    running: gifImage.enabled
                    to: gifImage.enabled ? 1 : 0
                    duration: 400
                    easing.type: Easing.OutQuart
                }
            }

            // Fallback text for items without images
            Text {
                text: root.title
                font.family: App.settings.font
                font.pointSize: 16
                visible: !primaryImage.visible && root.hasLicense
                color: Material.primaryTextColor
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
            }

            Image {
                id: audioIcon
                width: 20
                height: 20
                opacity: 0.25
                visible: root.containsAudio
                source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_contains_audio.svg"
                sourceSize: Qt.size(20, 20)
                asynchronous: true
                anchors {
                    top: parent.top
                    left: typeIcon.right
                    margins: 10
                }
            }

            Image {
                id: typeIcon
                width: 20
                height: 20
                opacity: 0.25
                source: root.typeIconSource
                sourceSize: Qt.size(20, 20)
                cache: true
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 10
                }
            }

            Rectangle {
                width: 120
                height: 20
                visible: root.isNew
                color: Material.color(Material.Orange)

                anchors {
                    top: parent.top
                    right: parent.right
                    rightMargin: -60
                    topMargin: -20
                }

                transform: Rotation { angle: 45 }

                Text {
                    font.family: App.settings.font
                    font.pointSize: 9
                    renderType: Text.QtRendering
                    color: "white"
                    text: qsTr("NEW")
                    smooth: true
                    antialiasing: true
                    anchors.centerIn: parent
                }
            }

            // License lock indicator
            ToolButton {
                enabled: false
                visible: !root.hasLicense
                icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/font-awsome/lock-solid.svg"
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

        MouseArea {
            anchors.fill: parent
            enabled: !root.isScrolling
            // hoverEnabled: !root.isScrolling
            // // cursorShape also changes even when hoverArea is disabled
            // cursorShape: root.isScrolling ? Qt.ArrowCursor : Qt.PointingHandCursor
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: function(mouse) {
                if (!root.hasLicense) {
                    root.openOpenLicensePopup();
                    return;
                }

                if (mouse.button === Qt.LeftButton && !App.util.isWidget(root.type)) {
                    root.clicked(root.folderName, root.type);
                } else if (mouse.button === Qt.RightButton) {
                    root.openContextMenu(Qt.point(mouseX, mouseY));
                }
            }
        }

        Button {
            id: widgetButton
            text: qsTr("Start")
            visible: widgetButton.enabled
            opacity: hoverArea.hovered ? 1:0
            enabled: App.util.isWidget(root.type) && hoverArea.hovered

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 5
            }

            onClicked: {
                App.screenPlayManager.startWidget(
                    root.type,
                    Qt.point(0, 0),
                    root.absoluteStoragePath,
                    root.preview,
                    {},
                    true
                );
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
            when: hoverArea.hovered && root.enableAnimations

            PropertyChanges {
                animatedContainer.scale: 1.05
            }
            PropertyChanges {
                typeIcon.opacity: 0.8
            }
            PropertyChanges {
                audioIcon.opacity: 0.8
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
                    target: typeIcon
                    duration: 300
                    easing.type: Easing.OutQuart
                }

                OpacityAnimator {
                    target: audioIcon
                    duration: 300
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
                      // Use wave effect for initial load
                      return Math.max(0, (root.rowIndex * 100) + (root.columnIndex * 50));
                  } else {
                      // For scrolled items, use minimal delay or no delay
                      return Math.max(0, root.columnIndex * 100); // Just a small column-based delay
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
