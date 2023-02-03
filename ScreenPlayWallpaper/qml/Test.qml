import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Particles
import QtQuick.Shapes
import ScreenPlayWallpaper

Rectangle {
    id: root

    property int attStrength: 800000
    //Emitter
    property bool isEnabled: true
    property int emitRate: 250
    property int lifeSpan: 5000
    property int size: 4
    property int endSize: 8
    property int sizeVariation: 4
    //Image
    property real imgOpacity: 0.75

    anchors.fill: parent
    color: Material.color(Material.Grey, Material.Shade800)
    border.width: 10
    border.color: "orange"
    Component.onCompleted:  Wallpaper.requestFadeIn();

    MouseArea {

        // setPosition()
        id: ma

        function setPosition() {
            attractor.pointX = mouseX - 25;
            attractor.pointY = mouseY - 25;
            mouseDot.x = mouseX - mouseDot.center;
            mouseDot.y = mouseY - mouseDot.center;
        }

        anchors.fill: parent
        preventStealing: true
        propagateComposedEvents: true
        hoverEnabled: true
        Component.onCompleted: {
            attractor.pointX = parent.width * 0.5;
            attractor.pointY = parent.height * 0.5;
        }
        onPositionChanged: {
            setPosition();
        }
        onClicked: {
        }
    }

    Rectangle {
        id: mouseDot

        property int center: mouseDot.width * 0.5

        width: 10
        height: width
        radius: width
        z: 99
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
        width: parent.width
        height: parent.height * 0.5
        system: particleSystem
        emitRate: root.emitRate
        lifeSpan: root.lifeSpan
        lifeSpanVariation: 1000
        size: root.size
        endSize: root.endSize
        sizeVariation: root.sizeVariation

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }

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
        id: txtMousePos

        property int counter: 0

        text: attractor.pointY + " - " + attractor.pointX
        font.pointSize: 32
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: "white"

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: txtButtonConter.top
            bottomMargin: 20
        }
    }

    Text {
        id: txtButtonConter

        property int counter: 0

        text: txtButtonConter.counter
        font.pointSize: 32
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: "white"

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: name.top
            bottomMargin: 20
        }
    }

    Text {
        id: name

        text: qsTr("This is a empty test window. You can change the source in test.qml")
        font.pointSize: 32
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        anchors.centerIn: parent
        anchors.margins: 10
        color: "white"
    }

    Row {
        spacing: 20

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: name.bottom
            topMargin: 20
        }

        Button {
            highlighted: true
            text: qsTr("Click me! - 1")
            onClicked: {
                focus = false;
                focus = true;
                print("Button Clicked!");
                txtButtonConter.counter = txtButtonConter.counter - 1;
            }
        }

        Button {
            highlighted: true
            text: qsTr("Exit Wallpaper")
            onClicked: {
                focus = false;
                focus = true;
                print("Exit Wallpaper");
                Wallpaper.terminate();
            }
        }

        Button {
            highlighted: true
            focusPolicy: Qt.ClickFocus
            text: qsTr("Click me! +1")
            onClicked: {
                print("Button Clicked!");
                txtButtonConter.counter = txtButtonConter.counter + 1;
            }
        }
    }

    //    WebView {
    //        width: 1000
    //        height: 400
    //        url: "https://screen-play.app"

    //        anchors {
    //            horizontalCenter: parent.horizontalCenter
    //            bottom: parent.bottom
    //            bottomMargin: 50
    //        }

    //    }
    MultimediaView {
        width: 1000
        height: 400

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 50
        }
    }
}
