import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

import QtQuick.Controls.Material.impl 2.12
import ScreenPlay 1.0

Rectangle {
    id: root
    height: 40
    opacity: 0
    anchors {
        horizontalCenter: parent.horizontalCenter
        bottomMargin: -root.height
        bottom: parent.bottom
    }

    radius: 4
    color: Material.color(Material.LightGreen)
    layer.enabled: true
    layer.effect: ElevationEffect {
        elevation: 6
    }

    function open() {
        root.state = "in"
        closeTimer.start()
    }
    function close() {
        root.state = ""
    }

    Timer {
        id: closeTimer
        interval: 1500
        onTriggered: root.close()
    }

    Text {
        text: qsTr("Profile saved successfully!")
        color: "white"
        font.family: ScreenPlay.settings.font
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
