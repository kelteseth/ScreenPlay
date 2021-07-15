import QtQuick 2.11
import Qt5Compat.GraphicalEffects
import QtQuick.Window 2.0
import Qt.WebSockets 1.15
import QtWebEngine 1.8

Rectangle {
    id: root

    property string fullContentPath
    property real volume: 1
    property string fillMode: "Cover"
    property string type

    function getSetVideoCommand() {
        // TODO 30:
        // Currently wont work. Commit anyways til QtCreator and Qt work with js template literals
        var src = "";
        src += "var videoPlayer = document.getElementById('videoPlayer');";
        src += "var videoSource = document.getElementById('videoSource');";
        src += "videoSource.src = '" + root.fullContentPath + "';";
        src += "videoPlayer.load();";
        src += "videoPlayer.volume = " + root.volume + ";";
        src += "videoPlayer.setAttribute('style', 'object-fit :" + root.fillMode + ";');";
        src += "videoPlayer.play();";
        print(src);
        return src;
    }

    color: "#333333"
    Component.onCompleted: {
        WebEngine.settings.localContentCanAccessFileUrls = true;
        WebEngine.settings.localContentCanAccessRemoteUrls = true;
        WebEngine.settings.allowRunningInsecureContent = true;
        WebEngine.settings.accelerated2dCanvasEnabled = true;
        WebEngine.settings.javascriptCanOpenWindows = false;
        WebEngine.settings.showScrollBars = false;
        WebEngine.settings.playbackRequiresUserGesture = false;
        WebEngine.settings.focusOnNavigationEnabled = true;
    }

    WebSocket {
        id: socket

        url: "ws://127.0.0.1:16395"
        active: true
        onStatusChanged: {
            if (socket.status === WebSocket.Error)
                messageBox.text = "Error: " + socket.errorString;
            else if (socket.status === WebSocket.Open)
                socket.sendTextMessage("Hello World");
            else if (socket.status === WebSocket.Closed)
                messageBox.text += "Socket closed";
        }
        onTextMessageReceived: {
            var obj = JSON.parse(message);
            if (obj.command === "replace") {
                root.type = obj.type;
                root.fillMode = obj.fillMode;
                root.volume = obj.volume;
                root.fullContentPath = obj.absolutePath + "/" + obj.file;
                webView.setVideo();
            }
        }
    }

    WebEngineView {
        id: webView

        function setVideo() {
            webView.runJavaScript(root.getSetVideoCommand());
        }

        anchors.fill: parent
        opacity: loadProgress === 100 ? 1 : 0
        onLoadProgressChanged: {
            if (loadProgress === 100)
                setVideo();

        }
    }

    Rectangle {
        id: infoWrapper

        width: 300
        height: 200
        opacity: 0
        anchors.centerIn: parent

        Text {
            id: messageBox

            text: qsTr("text")
            anchors.centerIn: parent
        }

    }

}
