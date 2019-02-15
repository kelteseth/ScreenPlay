import QtQuick 2.12
import QtWebEngine 1.8

Rectangle {
    id: root
    anchors.fill: parent

    Connections {
        target: mainwindow

        onQmlExit: {

            webView.runJavaScript(
                        "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;")
            mainwindow.destroyThis()
        }

        onFillModeChanged: {

            //TODO
        }

        onQmlSceneValueReceived: {
            var obj2 = 'import QtQuick 2.12; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
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

        onPlaybackRateChanged: {
            if (webView.loadProgress === 100) {
                webView.runJavaScript(
                            "var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.playbackRate  = " + playbackRate + ";")
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
            mainwindow.init()
            timer.start()
        } else if (mainwindow.type === "video") {
            webView.visible = true
            webView.url = Qt.resolvedUrl(
                        "file:///" + mainwindow.getApplicationPath(
                            ) + "/index.html")
        } else if (mainwindow.type === "html") {
            webView.visible = true
            webView.url = Qt.resolvedUrl(
                        "file:///" + mainwindow.fullContentPath)
            mainwindow.init()
            timer.start()
        }
        mainwindow.init()
        timer.start()
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        onLoadProgressChanged: {
            if (loadProgress === 100) {
                runJavaScript(("
var videoPlayer = document.getElementById('videoPlayer');
var videoSource = document.getElementById('videoSource');
videoSource.src = \"file:///" + mainwindow.fullContentPath + "\";
videoPlayer.load();
videoPlayer.volume = " + mainwindow.volume + ";" +
" videoPlayer.play();"), function (result) {

})
            }
        }
        onJavaScriptConsoleMessage: {
            print(message)
//            runJavaScript("
//                var videoPlayer = document.getElementById('errorMsg');
//                var text = document.createTextNode("+message+");

//                videoPlayer.appendChild(text);

//");

        }

        settings.allowRunningInsecureContent: true
        settings.accelerated2dCanvasEnabled: true
        settings.javascriptCanOpenWindows: false
        settings.printElementBackgrounds: false
        settings.showScrollBars: false
        settings.playbackRequiresUserGesture: false
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
