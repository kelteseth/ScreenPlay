import QtQuick 2.11
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.15
import Qt.WebSockets 1.15
import QtWebEngine 1.8

Rectangle {
    id: root
    color: "orange"

    property string fullContentPath
    property real volume: 1
    property string fillMode: "Cover"
    property string type

    function getSetVideoCommand() {
        // TODO 30:
        // Currently wont work. Commit anyways til QtCreator and Qt work with js template literals
        var src = ""
        src += "var videoPlayer = document.getElementById('videoPlayer');"
        src += "var videoSource = document.getElementById('videoSource');"
        src += "videoSource.src = '" + root.fullContentPath + "';"
        src += "videoPlayer.load();"
        src += "videoPlayer.volume = " + root.volume + ";"
        src += "videoPlayer.setAttribute('style', 'object-fit :" + root.fillMode + ";');"
        src += "videoPlayer.play();"
        return src
    }
    Component.onCompleted: {
        WebEngine.settings.localContentCanAccessFileUrls = true
        WebEngine.settings.localContentCanAccessRemoteUrls = true
        WebEngine.settings.allowRunningInsecureContent = true
        WebEngine.settings.accelerated2dCanvasEnabled = true
        WebEngine.settings.javascriptCanOpenWindows = false
        WebEngine.settings.showScrollBars = false
        WebEngine.settings.playbackRequiresUserGesture = false
        WebEngine.settings.focusOnNavigationEnabled = true
    }

    WebSocket {
        id: socket

        url: "ws://127.0.0.1:16395"
        active: true
        onStatusChanged: {
             if (socket.status === WebSocket.Open)
                socket.sendTextMessage("Hello World from QML wallpaper")
        }

        onTextMessageReceived: {
            socket.sendTextMessage(message)
            print(message)

            var obj = JSON.parse(message)

            if (obj.command === "replace") {
                 socket.sendTextMessage("replace")
                root.type = obj.type
                //root.fillMode = obj.fillMode
                root.volume = obj.volume
                root.fullContentPath = obj.absolutePath + "/" + obj.file
                const msg = "JSON: " +root.type + root.fillMode + root.volume + root.fullContentPath;
                socket.sendTextMessage(root.getSetVideoCommand())
                webView.setVideo()
            }
        }
    }

    WebEngineView {
        id: webView

        function setVideo() {
            webView.runJavaScript(root.getSetVideoCommand());
        }
        url:"index.html"
        anchors.fill: parent
        opacity: loadProgress === 100 ? 1 : 0
    }
}
