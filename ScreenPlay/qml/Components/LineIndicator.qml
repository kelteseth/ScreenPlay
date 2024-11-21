import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls
import ScreenPlayCore as Util
import ScreenPlayApp

Rectangle {
    id: root
    z: selected ? 99 : 0
    property int index: 0
    property string identifier
    property bool selected: false // User selected
    property bool isActive: false // Active based on time
    property bool isLast: false
    property alias text: text.text
    property string wallpaperPreviewImage

    signal remove(int index)
    signal lineSelected(int index)

    function toString(): void {
        console.log(`LineIndicator {
        index: ${index}
        text: ${text}
        selected: ${selected}
        isActive: ${isActive}
        isLast: ${isLast}
        previewImage: ${wallpaperPreviewImage || 'none'}
    }`);
    }

    Text {
        id: text
        color: "white"
        //text: root.index
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10
        }
    }

    Util.RainbowGradient {
        id: inlineRainbowGradient
        height: root.height
        visible: root.isActive
        running: root.isActive
        anchors {
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }
    }

    Util.RainbowGradient {
        id: rainbowGradient
        visible: false
        running: root.isActive
        height: 15
        anchors {
            right: parent.right
            left: parent.left
            rightMargin: -10
            leftMargin: -10
            bottom: parent.bottom
            bottomMargin: parent.height
        }
    }
    MultiEffect {
        id: rainbowEffect
        source: rainbowGradient
        anchors.fill: rainbowGradient

        // Mask effect
        maskSpreadAtMax: .5
        maskSpreadAtMin: 1
        maskThresholdMax: 1
        maskThresholdMin: .5
        maskEnabled: true
        maskSource: Image {
            id: maskImage
            source: "qrc:/qml/ScreenPlayApp/assets/images/timeline_mask.png"
            width: rainbowGradient.width
            height: rainbowGradient.height
            fillMode: Image.Stretch
        }

        // Add these properties to your QML item
        property real breathingMin: 0.8
        property real breathingMax: 1.0

        // Modify the saturation calculation
        // saturation: breathingMin + (Math.sin(breathingAnimation.running ? breathingAnimation.currentValue : 0) * (breathingMax - breathingMin) / 2)
        NumberAnimation {
            id: breathingAnimation
            running: root.isActive
            from: 0
            to: Math.PI * 2
            duration: 3000
            loops: Animation.Infinite
        }
        // Opacity behavior
        opacity: root.isActive ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }
    }

    Rectangle {
        id: indicatorLineVertical
        width: 5
        height: root.selected ? 40 : 30
        color: root.selected ? "gold" : parent.color
        clip: root.isActive
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.bottom
            topMargin: 0
        }
        Behavior on height {
            NumberAnimation {
                duration: 200
            }
        }

        Behavior on color {

            ColorAnimation {
                duration: 200
            }
        }

        Util.RainbowGradient {
            width: root.width
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }

            visible: running
            running: root.isActive
        }
    }

    Util.RainbowGradient {
        id: monitorBackground
        width: 70
        height: 48
        radius: 5
        running: root.isActive/*
        border.width: root.selected ? 2 : 0
        border.color: "gold"*/
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: indicatorLineVertical.bottom
            topMargin: -1
        }

        Rectangle {
            color: root.color
            anchors.fill: parent
            visible: !root.isActive
            radius: 5
        }

        Rectangle {
            anchors.fill: parent
            // hardcode instead monitorBackground.border.width
            // so it does not jump on highlight
            anchors.margins: 3
            radius: 5
            clip: true
            color: Qt.darker(monitorBackground.color)

            Image {
                id: imgWallpaper
                anchors.fill: parent
                anchors.margins: -3
                asynchronous: true
                source: {
                    let imgSource = root.wallpaperPreviewImage;
                    if (imgSource === "") {
                        return "";
                    }
                    return imgSource;
                }

                onStatusChanged: {
                    if (status === Image.Error)
                        print(Qt.resolvedUrl(root.wallpaperPreviewImage));
                }
            }
        }

        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }
        MouseArea {
            enabled: !App.globalVariables.isBasicVersion()
            anchors.fill: parent
            hoverEnabled: !App.globalVariables.isBasicVersion()
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                root.lineSelected(root.index);
            }
            ToolTip.visible: containsMouse && !App.globalVariables.isBasicVersion()
            ToolTip.delay: 500
            ToolTip.text: "index: " + root.index + " - id:" + root.identifier
        }

        Item {
            id: buttonWrapper
            width: removeButton.width
            height: removeButton.height
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.bottom
                topMargin: -5
            }

            property int disabledClickCount: 0

            ToolButton {
                id: removeButton
                anchors.fill: parent
                text: "❌"
                visible: root.selected
                enabled: visible && !root.isLast
                onClicked: root.remove(root.index)
                font.pointSize: 10
                hoverEnabled: !App.globalVariables.isBasicVersion()
                opacity: hovered ? 1 : .5
                ToolTip.visible: root.isLast && root.selected && hovered
                ToolTip.delay: 1000
                ToolTip.text: qsTr("You cannot remove the last timeline. There must always be 🌩️ a timeline wallpaper.")
                Behavior on opacity {
                    NumberAnimation {
                        duration: 200
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                visible: root.isLast && root.selected && !App.globalVariables.isBasicVersion()
                onClicked: {
                    buttonWrapper.disabledClickCount++;
                    if (buttonWrapper.disabledClickCount >= 5) {
                        Qt.openUrlExternally("https://youtu.be/x0fm48LhJ9k?si=RYKjtOwMQJOhOkFC&t=73");
                        buttonWrapper.disabledClickCount = 0;
                    }
                }
            }
        }
    }
}
