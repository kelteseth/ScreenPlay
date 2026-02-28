// SPDX-License-Identifier: MIT
// Based on Partycles by Jonathan Leane – https://jonathanleane.github.io/partycles/
import QtQuick
import QtQuick.Particles

/*!
    \qmltype FireworksEffect
    \inqmlmodule ScreenPlayCore
    \brief A particle-based fireworks burst effect.

    Renders a mortar-shell fireworks effect that can be triggered
    at arbitrary screen coordinates via \l triggerBurst().

    \code
    FireworksEffect {
        anchors.fill: parent
        running: true
    }

    Timer {
        interval: 1500; repeat: true; running: parent.running
        onTriggered: fireworks.triggerBurst(Math.random() * width, Math.random() * height * 0.7)
    }
    \endcode
*/
Item {
    id: root

    property int particleCount: 60
    property real spread: 360
    property real startVelocity: 22
    property real elementSize: 7
    property int lifetime: 160
    property real gravity: 0.35
    property real friction: 0.985
    property bool twinkle: false
    property var colors: ["#FF4500", "#FF6600", "#FFAA00", "#FF0000", "#FF8800", "#FFD000"]

    property real _burstX: width / 2
    property real _burstY: height * 0.25

    /*!
        \qmlmethod void FireworksEffect::triggerBurst(real x, real y)
        Launches a mortar shell from the bottom of the item that explodes
        at (\a x, \a y).
    */
    function triggerBurst(x: real, y: real): void {
        root._burstX = x
        root._burstY = y
        mortar.x = x
        mortar.y = root.height
        mortar.burst(1)
        mortarTimer.restart()
    }

    ParticleSystem {
        id: sys
        running: true
    }

    // Rising mortar shell
    Emitter {
        id: mortar
        system: sys
        group: "shell"
        x: root.width / 2
        y: root.height
        emitRate: 0
        lifeSpan: 600
        size: 10
        sizeVariation: 2
        velocity: AngleDirection {
            angle: 270
            magnitude: root.startVelocity * 55
            magnitudeVariation: root.startVelocity * 5
        }
    }

    // Smoke trail following the shell
    TrailEmitter {
        system: sys
        follow: "shell"
        group: "smoke"
        emitRatePerParticle: 60
        lifeSpan: 400
        size: 8
        sizeVariation: 5
        velocity: AngleDirection {
            angle: 90
            magnitude: 10
            magnitudeVariation: 20
            angleVariation: 25
        }
    }

    // Burst sparks emitted at the apex
    Emitter {
        id: boom
        system: sys
        group: "sparks"
        x: root._burstX
        y: root._burstY
        emitRate: 0
        lifeSpan: root.lifetime * 16
        lifeSpanVariation: root.lifetime * 5
        size: root.elementSize
        sizeVariation: root.elementSize * 0.4
        velocity: AngleDirection {
            angle: 0
            angleVariation: 180
            magnitude: root.startVelocity * 56
            magnitudeVariation: root.startVelocity * 22
        }
    }

    Gravity {
        system: sys
        groups: ["sparks", "shell", "smoke"]
        angle: 90
        magnitude: root.gravity * 580
    }

    Friction {
        system: sys
        groups: ["sparks"]
        factor: (1.0 - root.friction) * 180
    }

    Wander {
        system: sys
        groups: ["smoke"]
        pace: 15
        xVariance: 20
        yVariance: 10
    }

    ItemParticle {
        system: sys
        groups: ["shell"]
        delegate: Component {
            Rectangle {
                width: 10
                height: 10
                radius: 5
                color: "#ffeeaa"
            }
        }
    }

    ItemParticle {
        system: sys
        groups: ["smoke"]
        delegate: Component {
            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: "#555566"
                opacity: 0.4
            }
        }
    }

    ItemParticle {
        system: sys
        groups: ["sparks"]
        delegate: Component {
            Rectangle {
                id: spark
                property real sz: root.elementSize * (0.5 + Math.random() * 0.9)
                property color sparkColor: root.colors[Math.floor(Math.random() * root.colors.length)]

                width: sz
                height: sz
                radius: sz / 2
                color: sparkColor
                opacity: root.twinkle ? 0.5 + Math.random() * 0.5 : 0.9

                // Soft glow halo
                Rectangle {
                    anchors.centerIn: parent
                    width: spark.sz * 3
                    height: width
                    radius: width / 2
                    z: -1
                    color: Qt.rgba(parseInt(spark.sparkColor.toString().slice(1, 3), 16) / 255, parseInt(spark.sparkColor.toString().slice(3, 5), 16) / 255, parseInt(spark.sparkColor.toString().slice(5, 7), 16) / 255, 0.2)
                }
            }
        }
    }

    // Fires the burst sparks after the shell has risen to the apex
    Timer {
        id: mortarTimer
        interval: 600
        onTriggered: {
            boom.x = root._burstX
            boom.y = root._burstY
            boom.burst(root.particleCount)
        }
    }
}
