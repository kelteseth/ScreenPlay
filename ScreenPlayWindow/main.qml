import QtQuick 2.9
import net.aimber.screenplaysdk 1.0

Rectangle {
    color: "gray"
    anchors.fill: parent


    ScreenPlaySDK {
        contentType: "wallpaper"

        onIncommingMessageError: {
            name.text = msg
        }

        onSdkConnected: {
            name.text = "connected"
        }

        onSdkDisconnected:  {
            name.text = "disconnected"
            mainwindow.destroyThis()
        }
    }

    Text {
        id: name
        text: qsTr("text")
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        y:-100

        font.pixelSize: 36
        SequentialAnimation {
            loops: SequentialAnimation.Infinite
            running: true
            NumberAnimation {
                target: name
                property: "y"
                duration: 2000
                easing.type: Easing.InOutQuad
                to:500
            }
            NumberAnimation {
                target: name
                property: "y"
                duration: 2000
                easing.type: Easing.InOutQuad
                to:300
            }
        }
    }
}
