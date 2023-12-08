import QtQuick
import QtWebEngine
import ScreenPlayWallpaper

/*!
  * The native macOS multimedia stack does not support VP8/VP9. For this we must use the WebEngine.
  */
Item {
    id: root

    function getSetVideoCommand() {
        // TODO 30:
        // Currently wont work. Commit anyways til QtCreator and Qt work with js template literals
        var src = "";
        src += "var videoPlayer = document.getElementById('videoPlayer');";
        src += "var videoSource = document.getElementById('videoSource');";
        src += "videoSource.src = '" + Wallpaper.projectSourceFileAbsolute + "';";
        src += "videoPlayer.load();";
        src += "videoPlayer.volume = " + Wallpaper.volume + ";";
        src += "videoPlayer.setAttribute('style', 'object-fit :" + Wallpaper.fillMode + ";');";
        src += "videoPlayer.play();";
        print(src);
        return src;
    }

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

    WebEngineView {
        id: webView
        anchors.fill: parent
        url: Qt.resolvedUrl("file://" + Wallpaper.getApplicationPath() + "/index.html")
        onJavaScriptConsoleMessage: function (lineNumber, message) {
            print(lineNumber, message);
        }

        onLoadProgressChanged: {
            if (loadProgress === 100) {
                webView.runJavaScript(root.getSetVideoCommand(), function (result) {
                        Wallpaper.requestFadeIn();
                    });
            }
        }
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
        width: parent.width * 0.8
        color: "white"
    }

    Timer {
        id: timerCover

        interval: 300
        onTriggered: {
            webView.visible = !Wallpaper.visualsPaused;
            txtVisualsPaused.visible = Wallpaper.visualsPaused;
        }
    }

    Connections {
        function onReloadVideo(oldType) {
            webView.runJavaScript(root.getSetVideoCommand());
        }

        function onQmlExit() {
            webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;");
        }

        function onMutedChanged(muted) {
            if (muted)
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = 0;");
            else
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + Wallpaper.volume + ";");
        }

        function onFillModeChanged(fillMode) {
            if (webView.loadProgress === 100)
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.setAttribute('style', 'object-fit :" + fillMode + ";');");
        }

        function onLoopsChanged(loops) {
            if (webView.loadProgress === 100)
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.loop = " + loops + ";");
        }

        function onVolumeChanged(volume) {
            if (webView.loadProgress === 100)
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.volume = " + volume + ";");
        }

        function onCurrentTimeChanged(currentTime) {
            if (webView.loadProgress === 100)
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.currentTime  = " + currentTime + " * videoPlayer.duration;");
        }

        function onPlaybackRateChanged(playbackRate) {
            if (webView.loadProgress === 100)
                webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.playbackRate  = " + playbackRate + ";");
        }

        function onVisualsPausedChanged(visualsPaused) {
            if (visualsPaused) {
                // Wait until Wallpaper animation is finsihed
                timerCover.restart();
            } else {
                webView.visible = true;
                txtVisualsPaused.visible = false;
            }
        }

        function onIsPlayingChanged(isPlaying) {
            if (webView.loadProgress === 100) {
                if (isPlaying)
                    webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.play();");
                else
                    webView.runJavaScript("var videoPlayer = document.getElementById('videoPlayer'); videoPlayer.pause();");
            }
        }

        target: Wallpaper
    }
}
