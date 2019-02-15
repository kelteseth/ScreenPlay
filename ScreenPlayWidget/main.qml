import QtQuick 2.12
import net.aimber.screenplaysdk 1.0
import QtQuick.Controls 2.3

Item {
    id: mainWindow
    visible: true
    anchors.fill: parent

    Rectangle {
        id: bgColor
        anchors.fill: parent
        color: "white"
        //color: "#1A1F22"
        opacity: .15
    }

    Image {
        id: bg
        source: "qrc:/assets/image/noisy-texture-3.png"
        anchors.fill: parent
        opacity: .05
        fillMode: Image.Tile
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onPressed: {
            backend.setClickPos(Qt.point(mouse.x, mouse.y))
        }

        onPositionChanged: {
            backend.setPos(mouse.x, mouse.y)
        }
    }

    Connections {
        target: backend

        onSetWidgetSource: {
            loader.source = Qt.resolvedUrl("file:///" + source)
            print(loader.source)
        }

        onQmlSceneValueReceived: {
            var obj2 = 'import QtQuick 2.12; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
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
}
