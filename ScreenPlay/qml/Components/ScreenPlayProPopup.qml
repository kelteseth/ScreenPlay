pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls.Material
import QtQuick.Particles
import ScreenPlayCore as Util

Popup {
    id: root
    width: Math.min(Math.max(ApplicationWindow.window?.width ?? 800 - 20, ApplicationWindow.window?.maximumWidth ?? 400), 1000)
    height: Math.min(Math.max(ApplicationWindow.window?.height ?? 600 - 20, ApplicationWindow.window?.maximumHeight ?? 300), 500)
    anchors.centerIn: Overlay.overlay
    modal: true
    focus: true
    property Item modalSource

    onAboutToHide: {
        // Fixes the ugly transition with
        // our ModalBackgroundBlur on exit
        modal = false;
    }
    onAboutToShow: {
        modal = true;
    }

    // Mouse area for tracking cursor position
    MouseArea {
        id: mouseTracker
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.NoButton

        property real mouseX: 0
        property real mouseY: 0

        onPositionChanged: function (mouse) {
            mouseX = mouse.x;
            mouseY = mouse.y;
        }
    }

    Overlay.modal: Util.ModalBackgroundBlur {
        id: blurBg
        colorOverlayOpacity: .3
        blur: .5
        sourceItem: root.modalSource
    }

    background: Rectangle {
        color: Material.backgroundColor
        radius: 4
        Util.RainbowGradient {
            id: proRainbowGradient
            visible: root.visible
            running: root.visible
            anchors.fill: parent
            radius: 4
            clip: true
        }

        Rectangle {
            radius: 4
            color: Material.backgroundColor
            anchors.fill: parent
            anchors.margins: 3
        }

        Rectangle {
            id: skyGradient
            anchors {
                fill: parent
                margins: 3
            }
            radius: 4
            opacity: .2

            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#0a1a3f"
                } // Dark blue
                GradientStop {
                    position: 0.4
                    color: "#1f3b75"
                } // Medium blue
                GradientStop {
                    position: 0.7
                    color: "#3a5998"
                } // Lighter blue
                GradientStop {
                    position: 1.0
                    color: Material.backgroundColor
                } // Bottom matches material background
            }
        }
    }

    Util.RainbowGradient {
        id: bottomRainbowGradient
        visible: false
        running: root.visible
        anchors {
            fill: parent
            margins: 10
            bottomMargin: -10 // No idea why this is needed
        }
    }

    MultiEffect {
        id: rainbowEffect
        source: bottomRainbowGradient
        anchors.fill: bottomRainbowGradient
        maskSpreadAtMax: .5
        maskSpreadAtMin: 1
        maskThresholdMax: 1
        maskThresholdMin: .5
        maskEnabled: true
        maskSource: Image {
            id: maskImage
            source: "qrc:/qt/qml/ScreenPlay/assets/images/first_startup_mask.png"
            width: bottomRainbowGradient.width
            height: bottomRainbowGradient.height
            fillMode: Image.Stretch
        }

        SequentialAnimation on opacity {
            id: breathingAnimation
            running: true
            loops: Animation.Infinite

            NumberAnimation {
                from: 0.1
                to: 0.6
                duration: 3500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.6
                to: 0.1
                duration: 3500
                easing.type: Easing.InOutSine
            }
        }
    }

    ParticleSystem {
        id: particleSystem
        anchors.fill: parent
        clip: true
        opacity: root.visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }

        ImageParticle {
            groups: ["stars"]
            source: "qrc:/qt/qml/ScreenPlay/assets/images/star.png"
            color: "white"
            alpha: 1
            alphaVariation: 1
            rotationVariation: 360
            entryEffect: ImageParticle.Scale
        }

        // New emoji particles
        ItemParticle {
            groups: ["emojis"]
            delegate: Text {
                property var emojiList: ["✨", "💫", "⭐", "🌟", "💎", "🔥", "💖"]
                text: emojiList[Math.floor(Math.random() * emojiList.length)]
                font.pixelSize: 20
                color: "white"
            }
        }

        Emitter {
            id: starEmitter
            group: "stars"
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: -600
                topMargin: -500
            }
            width: 10
            height: 10
            emitRate: 20
            lifeSpan: 8000
            lifeSpanVariation: 1000
            size: 10
            sizeVariation: 4
            endSize: 4
            startTime: 2000

            velocity: PointDirection {
                x: -150
                y: 150
                xVariation: 70
                yVariation: 0
            }
        }

        Emitter {
            id: emojiEmitter
            group: "emojis"
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: -600
                topMargin: -500
            }
            width: 10
            height: 10
            emitRate: 1
            lifeSpan: 8000
            lifeSpanVariation: 1500
            size: 20
            sizeVariation: 3

            velocity: PointDirection {
                x: -150  // Same direction as stars
                y: 150   // Same direction as stars
                xVariation: 50
                yVariation: 30
            }
        }

        // Subtle mouse attractor for emojis only
        Attractor {
            groups: ["emojis"]
            x: mouseTracker.mouseX
            y: mouseTracker.mouseY
            strength: 0.3  // Subtle attraction strength
            affectedParameter: Attractor.Position
            proportionalToDistance: Attractor.InverseLinear
        }
    }

    RowLayout {
        anchors {
            fill: parent
            leftMargin: 40
            rightMargin: 40
        }
        spacing: 50

        ColumnLayout {
            spacing: 5
            Layout.fillWidth: true

            Label {
                text: qsTr("Keep ScreenPlay free and open source forever")
                color: Material.accentColor
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 26
                wrapMode: Text.WordWrap
            }

            Label {
                text: qsTr("Your Pro upgrade directly funds continuous development")
                color: Material.secondaryTextColor
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 16
                wrapMode: Text.WordWrap
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.topMargin: 5
                spacing: 12

                Label {
                    Layout.fillWidth: true
                    Layout.topMargin: 40
                    text: qsTr("🎮 Godot Engine 4.4 - Interactive 3D wallpapers")
                    wrapMode: Text.WordWrap
                    lineHeight: 1.2
                    font.pointSize: 16
                    color: Material.primaryTextColor
                }

                Label {
                    Layout.fillWidth: true
                    text: qsTr("📅 Smart Timeline - Wallpapers that change with time")
                    wrapMode: Text.WordWrap
                    lineHeight: 1.2
                    font.pointSize: 16
                    color: Material.primaryTextColor
                }

                Label {
                    Layout.fillWidth: true
                    text: qsTr("💖 Support independent open source development")
                    wrapMode: Text.WordWrap
                    lineHeight: 1.2
                    font.pointSize: 16
                    color: Material.primaryTextColor
                }

                Label {
                    Layout.fillWidth: true
                    text: qsTr("💝 €19.99 - Sustains open source for everyone")
                    wrapMode: Text.WordWrap
                    lineHeight: 1.2
                    font.pointSize: 16
                    color: Material.primaryTextColor
                }
            }

            RowLayout {
                Layout.topMargin: 30
                Layout.fillWidth: true

                Button {
                    text: qsTr("🚀 Get Pro Now")
                    highlighted: true
                    onClicked: {
                        root.close();
                    }
                }

                Button {
                    text: qsTr("Get Pro for free (Compile it yourself)")
                    onClicked: Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/building-screenplay/")
                }

                Button {
                    text: qsTr("Not Right Now")
                    onClicked: root.close()
                }
            }
        }

        Item {
            Layout.preferredWidth: ship.width
            Layout.preferredHeight: ship.height

            Image {
                id: ship
                source: "qrc:/qt/qml/ScreenPlay/assets/images/rocket_3d.png"

                SequentialAnimation {
                    running: true
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: ship
                        property: "y"
                        from: 0
                        to: 10
                        duration: 1500
                        easing.type: Easing.InOutQuad
                    }

                    NumberAnimation {
                        target: ship
                        property: "y"
                        from: 10
                        to: 0
                        duration: 1500
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    }
    ToolButton {
        width: 32
        height: width
        icon.width: 16
        icon.height: 16
        icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/font-awsome/close.svg"
        icon.color: Material.iconColor
        onClicked: root.close()

        anchors {
            top: parent.top
            right: parent.right
        }
    }
}
