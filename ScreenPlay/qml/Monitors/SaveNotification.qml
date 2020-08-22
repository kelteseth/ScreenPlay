import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

import QtQuick.Controls.Material.impl 2.12
import ScreenPlay 1.0

Drawer {
    id: root
    height: 40
    modal: false
    dim: false
    interactive: false
    edge: Qt.BottomEdge

    enter: Transition {
        SmoothedAnimation {
            velocity: 10
            easing.type: Easing.InOutQuart
        }
        PropertyAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: 250
        }
    }

    exit: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: 250
        }

        SmoothedAnimation {
            velocity: 10
            easing.type: Easing.InOutQuart
        }
    }

    background: Rectangle {
        radius: 4
        color: Material.color(Material.Orange)
        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 6
        }
    }

    onOpened: closeTimer.start()
    Timer {
        id: closeTimer
        interval: 1000
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
}
