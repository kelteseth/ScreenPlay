import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlay

Rectangle {
    id: root

    function open() {
        root.state = "in";
        closeTimer.start();
    }

    function close() {
        root.state = "";
    }

    height: 40
    opacity: 0
    radius: 4
    color: Material.color(Material.LightGreen)
    layer.enabled: true

    anchors {
        horizontalCenter: parent.horizontalCenter
        bottomMargin: -root.height
        bottom: parent.bottom
    }

    Timer {
        id: closeTimer

        interval: 1500
        onTriggered: root.close()
    }

    Text {
        text: qsTr("Profile saved successfully!")
        color: "white"
        font.family: App.settings.font
        font.pointSize: 14
        verticalAlignment: Qt.AlignVCenter

        anchors {
            top: parent.top
            topMargin: 5
            left: parent.left
            leftMargin: 20
            bottom: parent.bottom
            bottomMargin: 5
        }

    }

    layer.effect: ElevationEffect {
        elevation: 6
    }

    transitions: [
        Transition {
            from: ""
            to: "in"
            reversible: true

            PropertyAnimation {
                target: root
                properties: "opacity,anchors.bottomMargin"
                duration: 250
                easing.type: Easing.InOutQuart
            }

        }
    ]
    states: [
        State {
            name: "in"

            PropertyChanges {
                target: root
                anchors.bottomMargin: 10
                opacity: 1
            }

        }
    ]
}
