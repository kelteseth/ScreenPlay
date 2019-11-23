import QtQuick 2.0
import ScreenPlay.Wallpaper 1.0
import QtWebEngine 1.8

Item {
    property string url: webView.url

    signal requestFadeIn()

    Component.onCompleted: {

        WebEngine.settings.allowRunningInsecureContent = true
        WebEngine.settings.accelerated2dCanvasEnabled = true
        WebEngine.settings.javascriptCanOpenWindows = false
        WebEngine.settings.printElementBackgrounds = false
        WebEngine.settings.showScrollBars = false
        WebEngine.settings.playbackRequiresUserGesture = false
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        url: Qt.resolvedUrl(window.getApplicationPath() + "/index.html")
        onLoadProgressChanged: {
            if (loadProgress === 100) {

                // TODO 30:
                // Currently wont work. Commit anyways til QtCreator and Qt work with js template literals
                var src = ""
                src += "var videoPlayer = document.getElementById('videoPlayer');"
                src += "var videoSource = document.getElementById('videoSource');"
                src += "videoSource.src = '" + window.fullContentPath + "';"
                src += "videoPlayer.load();"
                src += "videoPlayer.volume = " + window.volume + ";"
                src += "videoPlayer.setAttribute('style', 'object-fit :" + window.fillMode + ";');"
                src += "videoPlayer.play();"

                webView.runJavaScript(src, function (result) {
                    fadeInTimer.start()

                })
            }
        }

        onJavaScriptConsoleMessage: print(lineNumber, message)
    }

    Timer {
        id: fadeInTimer
        interval: 500
        onTriggered: requestFadeIn()
    }

    Connections {
        target: window

        onQmlExit: {
            webView.runJavaScript(
                        "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
        }

        onMutedChanged: {
            if (muted) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
            } else {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + window.volume + ";")
            }
        }

        onFillModeChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.setAttribute('style', 'object-fit :" + window.fillMode + ";');")
            }
        }

        onLoopsChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.loop = " + loops + ";")
            }
        }

        onVolumeChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + volume + ";")
            }
        }

        onCurrentTimeChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.currentTime  = "
                            + currentTime + " * videoPlayer.duration;")
            }
        }

        onPlaybackRateChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.playbackRate  = " + playbackRate + ";")
            }
        }

        onIsPlayingChanged: {
            if (webView.loadProgress === 100) {
                if (isPlaying) {
                    webView.runJavaScript(
                                "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.play();")
                } else {
                    webView.runJavaScript(
                                "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.pause();")
                }
            }
        }
    }
}
