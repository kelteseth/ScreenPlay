import QtQuick 2.0
import QtQuick.Controls 2.12
import QtWebSockets 1.1
import QtWebEngine 1.8
import QtMultimedia 5.12
import Qt.labs.settings 1.1


Rectangle {
    id:root
    color:"orange"
    property bool connected: false

    Settings {
        id:settings
    }

    Component.onCompleted: {
        wallpaper.projectSourceFileAbsolute = settings.value("SP_projectSourceFileAbsolute","NULL")
//        if(root.projectSourceFileAbsolute === "NULL")
//            return

        wallpaper.type = settings.value("SP_type")
        wallpaper.fillMode = settings.value("SP_fillMode")
        wallpaper.volume = settings.value("SP_volume")
        wallpaper.play()
    }


    Wallpaper {
        id:wallpaper
        anchors.fill: parent
       // visible: root.connected

        onFullContentPathChanged: settings.setValue("SP_fullContentPath",fullContentPath)
        onVolumeChanged: settings.setValue("SP_volume",volume)
        onFillModeChanged: settings.setValue("SP_fillMode",fillMode)
        onTypeChanged: settings.setValue("SP_type",type)
        onProjectSourceFileAbsoluteChanged: settings.setValue("SP_projectSourceFileAbsolute",projectSourceFileAbsolute)
        onLoopsChanged: settings.setValue("SP_loops",loops)

    }

    Timer {
        id:reconnectTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            if (socket.status === WebSocket.Open)
                return
            socket.active = false
            socket.active = true
            reconnectTimer.retryCounter += 1
        }
        property int retryCounter: 0
    }

    WebSocket {
        id: socket
        url: "ws://127.0.0.1:16395"
        onStatusChanged: {
             if (socket.status === WebSocket.Open)
                socket.sendTextMessage("Hello World from QML wallpaper")
        }

        onTextMessageReceived: (message)=> {

            var obj = JSON.parse(message)
            root.connected = true
            txtCommand.text = obj.command

            if (obj.command === "replace") {
                socket.sendTextMessage("replace")
                wallpaper.type = obj.type
                wallpaper.fillMode = obj.fillMode
                wallpaper.volume = obj.volume
                wallpaper.projectSourceFileAbsolute = "file://" + obj.absolutePath + "/" + obj.file
                print("got: " + root.projectSourceFileAbsolute)
                wallpaper.play()
                return;
           }
           if(obj.command === "quit"){
               wallpaper.stop()
           }
        }
    }
//    WaitingForScreenplay {
//        anchors.fill: parent
//        visible: !root.connected
//    }
    Column {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 60
        }
        Text {
            id:txtCommand
            color: "white"
        }
        Text {
            color: "white"
            text:wallpaper.type  +  wallpaper.projectSourceFileAbsolute
        }
        Text {
            color: "white"
            text:"Actitve: " +socket.active +" status: "+ socket.status + " reconnectTimer.retryCounter : "+ reconnectTimer.retryCounter
        }
    }



}

