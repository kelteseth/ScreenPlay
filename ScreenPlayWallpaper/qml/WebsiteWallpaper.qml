import QtQuick
import QtWebEngine
import ScreenPlayWallpaper

Item {
    id: root

    property string url

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
        url: Qt.resolvedUrl(root.url)
        onLoadProgressChanged: {
            if (loadProgress === 100)
                Wallpaper.requestFadeIn();
        }
    }
}
