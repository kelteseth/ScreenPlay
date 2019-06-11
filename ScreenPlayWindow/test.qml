import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.12
import QtQuick.Shapes 1.12

Rectangle {
    id: root
    anchors.fill: parent
    color: Material.color(Material.Grey, Material.Shade800)

    property int attStrength: 800000
    //Emitter
    property bool isEnabled: true
    property int emitRate: 250
    property int lifeSpan: 5000
    property int size: 4
    property int endSize: 8
    property int sizeVariation: 4

    //Image
    property real imgOpacity: .75

    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        Component.onCompleted: {
            attractor.pointX = parent.width * .5
            attractor.pointY = 0
        }

        onPositionChanged: {
            setPosition()
            if (ma.pressed) {
            }
        }
        onClicked: {
            setPosition()
        }
        function setPosition() {
            attractor.pointX = mouseX - 25
            attractor.pointY = mouseY - 25
            mouseDot.x = mouseX - mouseDot.center
            mouseDot.y = mouseY - mouseDot.center
        }
    }
    Rectangle {
        id: mouseDot
        property int center: mouseDot.width * .5
        width: 10
        visible: false
        height: width
        radius: width
        color: "orange"
    }

    Attractor {
        id: attractor
        system: particleSystem
        affectedParameter: Attractor.Acceleration
        strength: root.attStrength
        proportionalToDistance: Attractor.InverseQuadratic
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
        size: root.size
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

    Text {
        id: name
        text: qsTr("This is a empty test window. You can change the source in test.qml")
        font.pointSize: 32
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        anchors.fill: parent
        anchors.margins: 10
        color: "white"
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/

