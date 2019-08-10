import QtQuick 2.12
import QtWebEngine 1.8
import ScreenPlay.Wallpaper 1.0

Rectangle {
    id: root
    anchors.fill: parent
    color: {
        if (desktopProperties.color === null) {
            return "black"
        } else {
            return desktopProperties.color
        }
    }

    property bool canFadeIn: true

    Component.onCompleted: {

        WebEngine.settings.allowRunningInsecureContent = true
        WebEngine.settings.accelerated2dCanvasEnabled = true
        WebEngine.settings.javascriptCanOpenWindows = false
        WebEngine.settings.printElementBackgrounds = false
        WebEngine.settings.showScrollBars = false
        WebEngine.settings.playbackRequiresUserGesture = false

        switch (window.type) {
        case Wallpaper.WallpaperType.Video:
            webView.url = Qt.resolvedUrl(window.getApplicationPath(
                                             ) + "/index.html")
            webView.enabled = true

            break
        case Wallpaper.WallpaperType.Html:
            webView.enabled = true
            webView.url = Qt.resolvedUrl(window.fullContentPath)
            break
        case Wallpaper.WallpaperType.ThreeJSScene:
            webView.enabled = true
            break
        case Wallpaper.WallpaperType.Qml:
            loader.enabled = true
            loader.source = Qt.resolvedUrl(window.fullContentPath)
            fadeIn()
            break
        }
    }

    function fadeIn() {
        window.setVisible(true)
        if (canFadeIn) {
            animFadeIn.start()
        } else {
            imgCover.opacity = 0
        }
    }

    Timer {
        id: fadeInTimer
        interval: 50
        onTriggered: fadeIn()
    }

    OpacityAnimator {
        id: animFadeIn
        target: imgCover
        from: 1
        to: 0
        duration: 800
        easing.type: Easing.InOutQuad
    }

    OpacityAnimator {
        id: animFadeOut
        target: imgCover
        from: 0
        to: 1
        duration: 800
        easing.type: Easing.InOutQuad
        onFinished: window.terminate()
    }

    WebEngineView {
        id: webView
        enabled: false
        anchors.fill: parent
        onLoadProgressChanged: {
            if (loadProgress === 100) {

                var src = ""
                src += "var videoPlayer = document.getElementById('videoPlayer');"
                src += "var videoSource = document.getElementById('videoSource');"
                src += "videoSource.src = '" + window.fullContentPath + "';"
                src += "videoPlayer.load();"
                src += "videoPlayer.volume = " + window.volume + ";"
                src += "videoPlayer.play();"

                webView.runJavaScript(src, function (result) {
                    fadeInTimer.start()
                })
            }
        }

        onJavaScriptConsoleMessage: print(lineNumber, message)
    }

    Loader {
        id: loader
        anchors.fill: parent
        enabled: false
    }

    Image {
        id: imgCover
        anchors {
            top: parent.top
            topMargin: {
                if(desktopProperties.windowsVersion >= 1903){

                    var ratio = root.width / root.height
                    ratio = Math.round(ratio * 100) / 100

                    // 4:3
                    if (ratio === 1,3) {

                    }
                    // 16:10
                    if (ratio === 1,6) {

                    }
                    // 16:9
                    if (ratio === 1,77) {
                        return -(root.height / 9)
                    }
                    // 21:9
                    if (ratio === 2,37) {
                        return -((root.height / 9) / 3)
                    }
                    // 32:9
                    if (ratio === 3,55) {

                    }

                } else {
                    return 0;
                }
            }

            left: parent.left
            right: parent.right
        }

        sourceSize.width: root.width
        source: Qt.resolvedUrl("file:///" + desktopProperties.wallpaperPath)

        Component.onCompleted: {

            switch (desktopProperties.wallpaperStyle) {
            case 10:
                imgCover.fillMode = Image.PreserveAspectCrop
                break
            case 6:
                imgCover.fillMode = Image.PreserveAspectFit
                break
            case 2:
                break
            case 0:
                if (desktopProperties.isTiled) {
                    // Tiled
                    imgCover.fillMode = Image.Tile
                } else {
                    // Center
                    imgCover.fillMode = Image.PreserveAspectFit
                    imgCover.anchors.centerIn = parent
                    imgCover.width = sourceSize.width
                    imgCover.height = sourceSize.height
                }
                break
            case 22:
                canFadeIn = false
                break
            }
        }
    }

    Connections {
        target: window

        onMutedChanged:{
            if(muted){
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
            } else {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + window.volume + ";")
            }
        }

        onQmlExit: {
            webView.runJavaScript(
                        "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
            animFadeOut.start()
        }

        onQmlSceneValueReceived: {
            var obj2 = 'import QtQuick 2.0; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
            print(key, value)
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
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
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.currentTime  = " + currentTime + " * videoPlayer.duration;")
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
