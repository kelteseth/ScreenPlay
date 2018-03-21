import QtQuick 2.9
import QtQuick.Window 2.2
import net.aimber.screenplaysdk 1.0

Window {
    id: mainWindow
    visible: true
    width: 250
    height: 100
    color: "transparent"
    flags: Qt.SplashScreen | Qt.ToolTip | Qt.WindowStaysOnBottomHint

    ScreenPlaySDK {
        id: spSDK
        contentType: "ScreenPlayWindow"
        appID: mainwindow.appID

        onIncommingMessageError: {

        }
        onIncommingMessage: {
            var obj2 = 'import QtQuick 2.9; Item {Component.onCompleted: sceneLoader.item.'
                    + key + ' = ' + value + '; }'
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
        }

        onSdkConnected: {

        }

        onSdkDisconnected: {
            screenVideo.state = "destroy"
        }
    }

    Connections {
        target: mainWindow

        onSetWidgetSource: {
            loader.source = Qt.resolvedUrl("file:///" + source)
            print(loader.source)
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
    }
}
