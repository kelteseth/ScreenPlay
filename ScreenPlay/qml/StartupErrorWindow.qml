import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: window
    visible: true
    height: 500
    width: 400
    onClosing: Qt.quit()

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2)
        setY(Screen.height / 2 - height / 2)
    }

    LinearGradient {
        id: gradient
        anchors.fill: parent
        cached: true
        start: Qt.point(0, 0)
        end: Qt.point(gradient.width, gradient.height)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#FF9700"
                SequentialAnimation on color {
                    loops: Animation.Infinite
                    ColorAnimation {
                        from: "#FF9700"
                        to: "#F83C3C"
                        duration: 5000
                    }
                    ColorAnimation {
                        from: "#F83C3C"
                        to: "#FF9700"
                        duration: 5000
                    }
                }
            }
            GradientStop {
                position: 1.0
                color: "#F83C3C"
                SequentialAnimation on color {
                    loops: Animation.Infinite

                    ColorAnimation {
                        from: "#F83C3C"
                        to: "#FF9700"
                        duration: 100000
                    }
                    ColorAnimation {
                        from: "#FF9700"
                        to: "#F83C3C"
                        duration: 100000
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onPressed: {
                attractor.enabled = true
            }
            onPositionChanged: {
                if (ma.pressed) {
                    attractor.pointX = mouseX
                    attractor.pointY = mouseY
                }
            }
            onReleased: {
                attractor.enabled = false
            }
        }

        Attractor {
            id: attractor
            system: particleSystem
            affectedParameter: Attractor.Acceleration
            strength: 8000000
            proportionalToDistance: Attractor.InverseQuadratic
        }

        ParticleSystem {
            id: particleSystem
        }

        Emitter {
            id: emitter
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            width: parent.width
            height: 80
            system: particleSystem
            emitRate: 25
            lifeSpan: 5000
            lifeSpanVariation: 1000
            size: 8
            endSize: 18
            sizeVariation: 4
            velocity: AngleDirection {
                angle: -90
                magnitude: 50
                magnitudeVariation: 25
                angleVariation: 10
            }
        }

        ImageParticle {
            height: 16
            width: 16
            source: "qrc:/assets/particle/dot.png"
            system: particleSystem
            opacity: .5
        }

        Image {
            id: bgGlow
            width: 500
            height: width
            opacity: .3
            source: "qrc:/assets/particle/backgroundGlow.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: -width * .65
            }
        }
    }

    Text {
        id: name
        text: qsTr("Could not connect to Steam :(")
        color: "white"
        font.family: "Roboto"
        renderType: Text.NativeRendering
        wrapMode: Text.WordWrap
        font.pixelSize: 25
        anchors {
            top: parent.top
            margins: 30
            right: parent.right
            left: parent.left
        }
    }

    Text {
        id: txtDescription
        text: qsTr("To use ScreenPlay with all its features you have to have Steam running!")
        color: "white"
        font.family: "Roboto"
        renderType: Text.NativeRendering
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 18
        anchors {
            top: name.bottom
            margins: 30
            right: parent.right
            left: parent.left
        }
    }

    ColumnLayout {
        spacing: 10
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10
        }

        Button {
            text: qsTr("Close ScreenPlay")
            Material.background: Material.Red
            Material.foreground: "white"
            Layout.alignment: Qt.AlignCenter
            onClicked: Qt.quit()
        }
        Button {
            text: qsTr("Continue using ScreenPlay offline")
            Material.background: Material.Orange
            Material.foreground: "white"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                suError.startScreenPlayAnyway()
                window.close()
            }
        }
        Button {
            text: qsTr("Seek help in our greate community!")
            Material.background: Material.LightGreen
            Material.foreground: "white"
            Layout.alignment: Qt.AlignCenter
            onClicked: Qt.openUrlExternally("https://forum.screen-play.app")
        }
    }
}
