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
        onFillModeChanged: {

        }
        onLoopsChanged: {

        }
        onTypeChanged: {
            print(mainwindow.type)
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

    Component.onCompleted: {
        if (mainwindow.type === "qmlScene") {
            loader.setSource(Qt.resolvedUrl(
                                 "file:///" + mainwindow.fullContentPath))
            print("LOADING QMLSCENE" + loader.source)
            mainwindow.init()
            timer.start()
        } else if (mainwindow.type === "video") {
            webView.visible = true
            webView.url = Qt.resolvedUrl("file:///" + mainwindow.getApplicationPath(
                                                   ) + "/index.html")
        }
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        visible: false

        onLoadProgressChanged: {
            if (loadProgress === 100) {
                runJavaScript(("
var videoPlayer = document.getElementById('videoPlayer');
var videoSource = document.getElementById('videoSource');
videoSource.src = \"file:///" + mainwindow.fullContentPath + "\";
videoPlayer.load();
videoPlayer.volume = " + mainwindow.volume + ";"), function (result) {
    mainwindow.init()
    timer.start()
})
            }
        }
        onJavaScriptConsoleMessage: print(message)
        settings.allowRunningInsecureContent: true
    }

    Loader {
        id: loader
        anchors.fill: parent
        onStatusChanged: {
            print(webViewWrapper.errorString())
        }
    }

    Timer {
        id: timer
        interval: 200
        onTriggered: {
            anim.start()
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
