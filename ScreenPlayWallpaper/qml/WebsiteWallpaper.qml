import QtQuick 2.14
import QtWebEngine 1.8
import ScreenPlayWallpaper 1.0

Item {
    id: root

    property string url

    signal requestFadeIn()

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
        url: root.url
        onJavaScriptConsoleMessage: print(lineNumber, message)
        onLoadProgressChanged: {
            if ((loadProgress === 100))
                root.requestFadeIn();

        }
    }

}
