import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

import "Wizards/CreateWallpaper"

Item {
    id: particleSystemWrapper
    Image {
        id: backgroundNoisePattern
        source: "qrc:/assets/images/noisy-texture-3.png"
        anchors.fill: parent
        fillMode: Image.Tile
        opacity: .3
    }
    
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
        color: "orange"
        source: "qrc:/assets/particle/dot.png"
        system: particleSystem
        opacity: .75
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
