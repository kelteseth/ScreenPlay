import QtQuick 2.0
import QtWebEngine 1.8

import ScreenPlay.Wallpaper 1.0

Item {
    id: root
    property alias url: webView.url

    signal requestFadeIn

    Component.onCompleted: {
        WebEngine.settings.localContentCanAccessFileUrls = true
        WebEngine.settings.localContentCanAccessRemoteUrls = true
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
        url: {

            if (window.type === Wallpaper.WallpaperType.Video) {
                return Qt.resolvedUrl(window.getApplicationPath() + "/index.html")
            }
            if (window.type === Wallpaper.WallpaperType.Html) {
                return Qt.resolvedUrl(window.fullContentPath + "/index.html")
            }
        }
        onJavaScriptConsoleMessage: print(lineNumber, message)
        onLoadProgressChanged: {
            if ((loadProgress === 100)) {

                if (window.type === Wallpaper.WallpaperType.Video) {
                    // TODO 30:
                    // Currently wont work. Commit anyways til QtCreator and Qt work with js template literals
                    var src = ""
                    src += "var videoPlayer = document.getElementById('videoPlayer');"
                    src += "var videoSource = document.getElementById('videoSource');"
                    src += "videoSource.src = '" + window.fullContentPath + "';"
                    src += "videoPlayer.load();"
                    src += "videoPlayer.volume = " + window.volume + ";"
                    src += "videoPlayer.setAttribute('style', 'object-fit :"
                            + window.fillMode + ";');"
                    src += "videoPlayer.play();"

                    webView.runJavaScript(src, function (result) {
                        fadeInTimer.start()
                    })
                } else {
                    fadeInTimer.start()
                }
            }
        }
    }

    Timer {
        id: fadeInTimer
        interval: 500
        onTriggered: requestFadeIn()
    }

    Text {
        id: txtVisualsPaused
        text: qsTr("If you can read this, then the VisualsPaused optimization does not work on your system. You can fix this by disable this in: \n Settings -> Perfromance -> Pause wallpaper video rendering while another app is in the foreground ")
        font.pointSize: 32
        visible: false
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.centerIn: parent

        width: parent.width * .8
        color: "white"
    }


    Timer {
        id: timerCover
        interval: 300
        onTriggered: {
            webView.visible = !window.visualsPaused
            txtVisualsPaused.visible = window.visualsPaused
        }
    }


    Connections {
        target: window

        function onQmlExit() {
            webView.runJavaScript(
                        "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
        }

        function onMutedChanged(muted) {
            if (muted) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
            } else {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + window.volume + ";")
            }
        }

        function onFillModeChanged(fillMode) {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.setAttribute('style', 'object-fit :" + fillMode + ";');")
            }
        }

        function onLoopsChanged(loops) {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.loop = " + loops + ";")
            }
        }

        function onVolumeChanged(volume) {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + volume + ";")
            }
        }

        function onCurrentTimeChanged(currentTime) {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.currentTime  = "
                            + currentTime + " * videoPlayer.duration;")
            }
        }

        function onPlaybackRateChanged(playbackRate) {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.playbackRate  = " + playbackRate + ";")
            }
        }

        function onVisualsPausedChanged(visualsPaused) {
            if (visualsPaused) {
                // Wait until window animation is finsihed
                timerCover.restart()
            } else {
                webView.visible = true
                txtVisualsPaused.visible = false
            }
        }

        function onIsPlayingChanged(isPlaying) {
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
