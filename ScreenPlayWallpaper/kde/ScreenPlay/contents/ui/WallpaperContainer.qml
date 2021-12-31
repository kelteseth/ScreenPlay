import QtQuick 2.0
import QtQuick.Controls 2.12
import QtWebSockets 1.1
import QtWebEngine 1.8
import QtMultimedia 5.12


Rectangle {
    id: container
    anchors.fill: parent

    Loader {
        id: wp
        anchors.fill: parent
        source: "Wallpaper.qml"
        property bool connected: false

        Timer {
           id: connectTimer
           interval: 1000
           running: true
           repeat: true
           onTriggered: {
               if(!wp.connected){
                   console.log("not connected")
                   wp.source = ""
                   wp.source = "Wallpaper.qml"
               } else {
                   console.log("connected")
                   screensaver.visible = false
                   connectTimer.stop()
               }
           }
        }
    }

    Loader {
        anchors.fill: parent
        id: screensaver
        source: "WaitingForScreenplay.qml"
    }
}
