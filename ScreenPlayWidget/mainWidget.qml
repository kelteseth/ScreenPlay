import QtQuick 2.12
import QtQuick.Controls 2.3

Item {
    id: mainWindow
    anchors.fill: parent

    OpacityAnimator {
        id: animFadeOut
        from: 1
        to: 0
        target: parent
        duration: 800
        easing.type: Easing.InOutQuad
        onFinished: window.destroyThis()
    }

    Rectangle {
        id: bgColor
        anchors.fill: parent
        color: "white"
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

        Connections {
            target: loader.item
            onSizeChanged: {
                mainWindow.width = size.width
                mainWindow.height = size.height
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            window.setClickPos(Qt.point(mouse.x, mouse.y))
        }

        onPositionChanged: {
            if (mouseArea.pressed)
                window.setPos(mouse.x, mouse.y)
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

    MouseArea {
        id: mouseAreaClose
        width: 20
        height: width
        anchors {
            top: parent.top
            right: parent.right
        }
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            window.setWindowBlur(0)
            animFadeOut.start()
        }

        Image {
            source: "qrc:/assets/icons/baseline-close-24px.svg"
            anchors.centerIn: parent
        }
    }
    MouseArea {
        id: mouseAreaResize
        width: 20
        height: width
        anchors {
            bottom: parent.bottom
            right: parent.right
        }
        cursorShape: Qt.SizeFDiagCursor
        property point clickPosition

        onPressed: {
            clickPosition = Qt.point(mouseX, mouseY)
        }

        onPositionChanged: {
            if (mouseAreaResize.pressed) {
                window.setWidgetSize(clickPosition.x + mouseX,
                                     clickPosition.y + mouseY)
            }
        }
    }
}
