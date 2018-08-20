import QtQuick 2.9
import QtWebEngine 1.6
import net.aimber.screenplaysdk 1.0

Rectangle {
    id: root
    color: "transparent"
    anchors.fill: parent
    property string tmpVideoPath
    property var jsonProjectFile

    ScreenPlaySDK {
        id: spSDK
        contentType: "ScreenPlayWindow"
        appID: mainwindow.appID

        onIncommingMessageError: {

        }
        onIncommingMessage: {


            //            var obj2 = 'import QtQuick 2.9; Item {Component.onCompleted: sceneLoader.item.' + key + ' = ' + value + '; }'
            //            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            //            newObject.destroy(10000)
        }

        onSdkConnected: {

        }

        onSdkDisconnected: {
            mainwindow.destroyThis()
        }
    }

    Connections {
        target: mainwindow

        onPlayVideo: {

        }
        onPlayQmlScene: {

        }

        onDecoderChanged: {

        }
        onFillModeChanged: {

        }
        onLoopsChanged: {

        }
        onVolumeChanged: {

        }
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        url: Qt.resolvedUrl("file:///" + mainwindow.getApplicationPath() + "/index.html")
        onLoadProgressChanged: {
            print(loadProgress)
            if (loadProgress === 100) {
                timerShowDelay.start()
            }
        }
        userScripts: [scriptPlayer]
        onJavaScriptConsoleMessage: print(message)
        settings.allowRunningInsecureContent: true

        WebEngineScript {
            id: scriptPlayer
            injectionPoint: WebEngineScript.DocumentReady
            worldId: WebEngineScript.MainWorld
            sourceCode: {
                return "var videoPlayer = document.getElementById('videoPlayer');
                        var videoSource = document.getElementById('videoSource');
                        videoSource.src = \"file:///" + mainwindow.fullContentPath + "\";
                        videoPlayer.load();"
            }
        }


    }

    Timer {
        id: timerShowDelay
        interval: 2000
        onTriggered: {
            mainwindow.init()
        }
    }
}
