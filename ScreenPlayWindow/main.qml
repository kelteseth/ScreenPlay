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
                runJavaScript("var videoPlayer = document.getElementById('videoPlayer');
                        var videoSource = document.getElementById('videoSource');
                        videoSource.src = \"file:///" + mainwindow.fullContentPath + "\";
                        videoPlayer.load();", function(result) {  mainwindow.init(); timer.start()});

            }
        }
        onJavaScriptConsoleMessage: print(message)
        settings.allowRunningInsecureContent: true
    }
    Timer {
        id:timer
        interval: 200
        onTriggered: {
            curtain.opacity = 0
            anim.start()
            print("start")
        }
    }

    Rectangle {
        id:curtain
        anchors.fill: parent
        color: "black"

        PropertyAnimation {
            id:anim
            property: "opacity"
            target: curtain
            from: "1"
            to: "0"
            duration: 300
        }
    }

}
