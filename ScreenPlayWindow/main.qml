import QtQuick 2.9
import QtWebEngine 1.6

Rectangle {
    id: root
    color: "transparent"
    anchors.fill: parent
    property string tmpVideoPath
    property var jsonProjectFile

    Connections {
        target: mainwindow

        onPlayVideo: {

        }
        onPlayQmlScene: {

        }

        onFillModeChanged: {

        }
        onLoopsChanged: {

        }
        onVolumeChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + volume + ";")
            }
        }
        onIsPlayingChanged: {
            if (webView.loadProgress === 100) {
                if (isPlaying === "false") {
                    webView.runJavaScript(
                                "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.play();")
                } else {
                    webView.runJavaScript(
                                "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.pause();")
                }
            }
        }
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        url: Qt.resolvedUrl("file:///" + mainwindow.getApplicationPath(
                                ) + "/index.html")
        onLoadProgressChanged: {
            if (loadProgress === 100) {
                runJavaScript(("
                var videoPlayer = document.getElementById('videoPlayer');
                var videoSource = document.getElementById('videoSource');
                videoSource.src = \"file:///" + mainwindow.fullContentPath + "\";
                videoPlayer.load();
                videoPlayer.volume = " + mainwindow.volume + ";"),
                function (result) {
                    mainwindow.init()
                    timer.start()
                })
            }
        }
        onJavaScriptConsoleMessage: print(message)
        settings.allowRunningInsecureContent: true
    }

    Timer {
        id: timer
        interval: 200
        onTriggered: {
            curtain.opacity = 0
            anim.start()
            print("start")
        }
    }

    Rectangle {
        id: curtain
        anchors.fill: parent
        color: "black"

        PropertyAnimation {
            id: anim
            property: "opacity"
            target: curtain
            from: "1"
            to: "0"
            duration: 300
        }
    }
}
