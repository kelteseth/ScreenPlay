import QtQuick 2.9
import net.aimber.screenplaysdk 1.0
import QtQuick.Window 2.3

Window {
    id: mainWindow
    visible: true
    width: 250
    height: 250
    flags: Qt.SplashScreen | Qt.ToolTip | Qt.SplashScreen

    ScreenPlaySDK {
        id: spSDK
        contentType: "ScreenPlayWindow"
        appID: backend.appID

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
            Qt.quit()
        }
    }

    Connections {
        target: backend

        onSetWidgetSource: {
            loader.source = Qt.resolvedUrl("file:///" + source)
            print(loader.source)
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
        onStatusChanged: {
            if (loader.status === Loader.Ready) {

            }
        }
    }
    MouseArea {
        property point clickPos: "1,1"
        anchors.fill: parent
        z:99

        onPressed: {
            clickPos = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged: {
            var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
            var new_x = mainWindow.x + delta.x
            var new_y = mainWindow.y + delta.y
            mainWindow.x = new_x
            mainWindow.y = new_y
        }
    }
}
