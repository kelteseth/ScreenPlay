import QtQuick 2.12
import QtQuick.Controls 2.3
import QtWebEngine 1.8
import ScreenPlayWidget 1.0
import ScreenPlay.Enums.InstalledType 1.0

Item {
    id: mainWindow
    anchors.fill: parent

    Connections {
        target: Widget

        function onQmlExit() {
            Widget.setWindowBlur(0)
            animFadeOut.start()
        }

        function onQmlSceneValueReceived(key, value) {
            var obj2 = 'import QtQuick 2.14; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
        }
        // Replace wallpaper with QML Scene
        function onReloadQML(oldType) {

            loader.sourceComponent = undefined
            loader.source = ""
            Widget.clearComponentCache()

            loader.source = Qt.resolvedUrl(Widget.projectSourceFileAbsolute)
        }


    }

    OpacityAnimator {
        id: animFadeOut
        from: 1
        to: 0
        target: parent
        duration: 800
        easing.type: Easing.InOutQuad
        onFinished: Widget.destroyThis()
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
        Component.onCompleted: {
            switch (Widget.type) {
            case InstalledType.QMLWidget:
                loader.source = Qt.resolvedUrl( Widget.projectSourceFileAbsolute)
                break
            case InstalledType.HTMLWidget:
                loader.sourceComponent = webViewComponent
                break
            }
        }
        onStatusChanged: {
            if (loader.status == Loader.Ready) {
                if (loader.item.widgetBackground !== undefined) {
                    bgColor.color = loader.item.widgetBackground
                }
                if (loader.item.widgetBackgroundOpacity !== undefined) {
                    bgColor.opacity = loader.item.widgetBackgroundOpacity
                }
                if (loader.item.widgetWidth !== undefined
                        && loader.item.widgetHeight !== undefined) {
                    Widget.setWidgetSize(loader.item.widgetWidth,
                                         loader.item.widgetHeight)
                }
            }
        }
    }

    Component {
        id: webViewComponent
        WebEngineView {
            id: webView
            backgroundColor: "transparent"
            anchors.fill: parent
            onJavaScriptConsoleMessage: print(lineNumber, message)
            Component.onCompleted: {
                webView.url = Qt.resolvedUrl(Widget.sourcePath)
            }
        }
    }

    MouseArea {
        id: mouseArea
        property var clickPos
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            clickPos = {
                "x": mouse.x,
                "y": mouse.y
            }
        }

        onPositionChanged: {
            if (mouseArea.pressed) {
                Widget.setPos(Widget.cursorPos().x - clickPos.x,
                              Widget.cursorPos().y - clickPos.y)
            }
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
            Widget.setWindowBlur(0)
            animFadeOut.start()
        }
        hoverEnabled: true
        onEntered: imgClose.opacity = 1
        onExited: imgClose.opacity = .15

        Image {
            id: imgClose
            source: "qrc:/assets/icons/baseline-close-24px.svg"
            anchors.centerIn: parent
            opacity: .15
            OpacityAnimator {
                target: parent
                duration: 300
            }
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
                Widget.setWidgetSize(clickPosition.x + mouseX,
                                     clickPosition.y + mouseY)
            }
        }
    }
}
