import QtQuick 2.12
import QtQuick.Controls 2.3

import ScreenPlay.screenplaysdk 1.0

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
    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
        source: {
             Qt.resolvedUrl(window.sourcePath)
        }

        onStatusChanged: {
            if (loader.status === Loader.Ready) {

            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onPressed: {
            window.setClickPos(Qt.point(mouse.x, mouse.y))
        }

        onPositionChanged: {
            window.setPos(mouse.x, mouse.y)
        }
        onClicked: {

            if (mouse.button === Qt.RightButton) {
                contextMenu.popup()
            }
        }
    }
    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Close Widget")
            onClicked: {
               Qt.quit()
            }
        }

    }

    Connections {
        target: window


        onQmlSceneValueReceived: {
            var obj2 = 'import QtQuick 2.12; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
        }
    }


}
