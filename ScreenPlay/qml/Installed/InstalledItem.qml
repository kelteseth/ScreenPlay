import QtQuick
import QtQuick.Effects
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore as Util
import QtQuick.Window


Item {
    id: root

    required property string title
    required property string folderName
    required property string absoluteStoragePath
    required property int type
    // Must be var to make it work wit 64bit ints
    required property var publishedFileID
    required property bool isScrolling
    required property bool isNew
    required property bool containsAudio
    required property string previewGIF
    required property string preview
    required property int index
    property bool hasLicense: {
        if (App.globalVariables.isBasicVersion() && root.type === Util.ContentTypes.InstalledType.GodotWallpaper) {
            return false;
        }
        return true;
    }

    signal clicked(var folderName, var type)
    signal openContextMenu(var point)
    signal openOpenLicensePopup

    width: 320
    height: 180
    onTypeChanged: {
        if (App.util.isWidget(type)) {
            icnType.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_widgets.svg";
            return;
        }
        if (App.util.isScene(type)) {
            icnType.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_code.svg";
            return;
        }
        if (App.util.isVideo(type)) {
            icnType.source = "qrc:/qt/qml/ScreenPlay/assets/icons/icon_movie.svg";
            return;
        }
    }

    Timer {
        running: true
        onTriggered: showAnim.start()
        interval: {
            var itemIndexMax = root.index;
            if (root.index > 30)
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
            target: effectWrapper
            from: 0
            to: 1
            duration: 800
            easing.type: Easing.OutCirc
        }
    }


    Item {
        id: effectWrapper
        anchors.fill: parent
        opacity: 0
    }

    Item {
        id: screenPlayItemWrapper
        width: 320
        height: 180
        opacity: 0

        Item {
            id: itemWrapper
            visible: false
            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                color: Material.backgroundColor
            }

            Text {
                text: root.title
                font.family: App.settings.font
                font.pointSize: 16
                visible: !screenPlayItemImage.visible && root.hasLicense
                color: Material.primaryTextColor
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
            }

            InstalledItemImage {
                id: screenPlayItemImage
                opacity: root.hasLicense ? 1 : 0.3
                anchors.fill: parent
                enabled: visible
                visible: root.preview !== "" || root.previewGIF !== ""
                sourceImage: root.preview
                sourceImageGIF: root.previewGIF
                type: root.type
                absoluteStoragePath: root.absoluteStoragePath
            }

            Image {
                id: icnAudio
                width: 20
                height: 20
                opacity: 0.25
                visible: root.containsAudio
                source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_contains_audio.svg"
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
                source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_movie.svg"
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

        MultiEffect {
            anchors.fill: parent
            source: itemWrapper
            // TODO: Mask no longer works when enable shadow
            // shadowEnabled: true
            // shadowBlur: 0.3
            // shadowColor: "black"
            // shadowVerticalOffset: 3
            maskEnabled: true
            maskSource: Image {
                source: "qrc:/qt/qml/ScreenPlay/assets/images/installed_item_mask.png"
                sourceSize: Qt.size(root.width, root.height)
                width: root.width
                height: root.height
            }
        }
        MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: !root.isScrolling && !showAnim.running && root.hasLicense
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onEntered: handleMouseEnter()
            onExited: handleMouseExit()
            onClicked: function (mouse) {
                if (!root.hasLicense) {
                    root.openOpenLicensePopup();
                    return;
                }
                if (App.util.isWidget(root.type))
                    return;
                if (mouse.button === Qt.LeftButton)
                    root.clicked(root.folderName, root.type);
                else if (mouse.button === Qt.RightButton)
                    root.openContextMenu(Qt.point(mouseX, mouseY));
            }
            function handleMouseEnter() {
                if (!root.hasLicense)
                    return;
                root.state = "hover";
                screenPlayItemImage.state = "hover";
                screenPlayItemImage.enter();
            }
            function handleMouseExit() {
                if (widgetStartButton.enabled && widgetStartButton.hovered)
                    return;
                root.state = "";
                screenPlayItemImage.state = "loaded";
                screenPlayItemImage.exit();
            }
        }

        Button {
            id: widgetStartButton
            enabled: App.util.isWidget(root.type)
            hoverEnabled: enabled
            text: qsTr("Start")
            opacity: enabled && (widgetStartButton.hovered || hoverArea.containsMouse) ? 1 : 0
            onClicked: {
                App.screenPlayManager.startWidget(root.type, Qt.point(0, 0), root.absoluteStoragePath, root.preview, {}, true);
            }
            onHoveredChanged: {
                if (hovered)
                    hoverArea.handleMouseEnter();
                else
                    hoverArea.handleMouseExit();
            }

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 5
            }
            Behavior on opacity {
                PropertyAnimation {
                    property: "opacity"
                    duration: 250
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
                duration: 300
                from: 1
                to: 1.05
                easing.type: Easing.OutQuart
            }

            ScaleAnimator {
                target: effectWrapper
                duration: 300
                from: 1
                to: 1.05
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnType
                duration: 300
                from: 0.25
                to: 0.8
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnAudio
                duration: 300
                from: 0.25
                to: 0.8
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: effectWrapper
                duration: 2000
                from: 0.4
                to: 1
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "hover"
            to: ""
            ScaleAnimator {
                target: screenPlayItemWrapper
                duration: 300
                from: 1.05
                to: 1
                easing.type: Easing.OutQuart
            }

            ScaleAnimator {
                target: effectWrapper
                duration: 300
                from: 1.05
                to: 1
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnType
                duration: 300
                from: 0.8
                to: 0.25
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: icnAudio
                duration: 300
                from: 0.8
                to: 0.25
                easing.type: Easing.OutQuart
            }

            OpacityAnimator {
                target: effectWrapper
                duration: 300
                from: 1
                to: 0.4
                easing.type: Easing.OutQuart
            }
        }
    ]
}
