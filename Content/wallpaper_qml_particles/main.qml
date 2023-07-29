// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Effects
import QtQuick.Particles

Item {
    id: root
    anchors.fill: parent

    property int attStrength: 8000000
    //Emitter
    property bool isEnabled: true
    property int emitRate: 25
    property int lifeSpan: 5000
    property int endSize: 8
    property int sizeVariation: 4
    property int particleSize: 16

    //Image
    property real imgOpacity: .75

    Rectangle {
        id: gradient
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#199EF1"
            }
            GradientStop {
                position: 1.0
                color: "#092E6C"
            }
        }
    }

    Item {
        anchors.fill: parent

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            Component.onCompleted: {
                attractor.pointX = parent.width * .5
                attractor.pointY = 0
            }

            onPressed: {
                attractor.enabled = true
            }
            onPositionChanged: {
                attractor.pointX = mouseX
                attractor.pointY = mouseY
            }
            onReleased: {
                attractor.enabled = false
            }
        }

        Attractor {
            id: attractor
            system: particleSystem
            affectedParameter: Attractor.Acceleration
            strength: root.attStrength
            proportionalToDistance: Attractor.InverseQuadratic
            anchors {
                top: parent.top
                topMargin: 100
                horizontalCenter: parent.horizontalCenter
            }
        }

        ParticleSystem {
            id: particleSystem
        }

        Emitter {
            id: emitter
            enabled: root.isEnabled
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            width: parent.width
            height: parent.height * .5
            system: particleSystem
            emitRate: root.emitRate
            lifeSpan: root.lifeSpan
            lifeSpanVariation: 1000
            size: root.particleSize
            endSize: root.endSize
            sizeVariation: root.sizeVariation
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
            source: "dot.png"
            system: particleSystem
            opacity: root.imgOpacity
        }

        Image {
            id: bgGlow
            width: parent.width * .75
            height: width
            opacity: .3
            source: "backgroundGlow.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: -width * .65
            }
            SequentialAnimation on opacity {
                loops: Animation.Infinite

                OpacityAnimator {
                    from: 0
                    to: .3
                    duration: 100000
                }
                OpacityAnimator {
                    from: .4
                    to: 0
                    duration: 100000
                }
            }
        }
    }
}
