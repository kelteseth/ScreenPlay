import QtQuick
import QtQuick.Controls
import QtWebEngine
import ScreenPlayWidget
import ScreenPlay.Enums.InstalledType
import ScreenPlayUtil as Util

Item {
    id: root
    // We must set a default size not not
    // trigger render errors.
    width: 200
    height: 100
    state: "out"
    opacity: 0

    Connections {
        target: Widget
        function onQmlExit() {
            if (Qt.platform.os === "windows")
                Widget.setWindowBlur(0);
            loader.source = "";
            animFadeOut.start();
        }

        function onQmlSceneValueReceived(key, value) {
            var obj2 = 'import QtQuick; Item {Component.onCompleted: loader.item.' + key + ' = ' + value + '; }';
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err");
            newObject.destroy(10000);
        }

        // Replace wallpaper with QML Scene
        function onReloadQML(oldType) {
            loader.sourceComponent = undefined;
            loader.source = "";
            Widget.clearComponentCache();
            loader.source = Qt.resolvedUrl(Widget.projectSourceFileAbsolute);
        }
    }

    OpacityAnimator {
        id: animFadeOut
        from: 1
        to: 0
        target: root
        duration: 800
        easing.type: Easing.InOutQuad
        onFinished: Widget.destroyThis()
    }

    Image {
        id: bgNoise
        source: "qrc:/qml/ScreenPlayWidget/assets/image/noisy-texture-3.png"
        anchors.fill: parent
        fillMode: Image.Tile
        opacity: 0
    }

    Connections {
        ignoreUnknownSignals: true
        target: loader.item
        // Some width do not know their implicit size,
        // see the xkcd example.
        function onImplicitWidthChanged() {
            print();
            root.width = loader.item.implicitWidth;
        }
        function onImplicitHeightChanged() {
            root.height = loader.item.implicitHeight;
        }
    }

    Loader {
        id: loader
        opacity: 0
        anchors.fill: parent
        asynchronous: true
        Component.onCompleted: {
            switch (Widget.type) {
            case InstalledType.QMLWidget:
                loader.source = Qt.resolvedUrl(Widget.projectSourceFileAbsolute);
                break;
            case InstalledType.HTMLWidget:
                loader.sourceComponent = webViewComponent;
                break;
            }
        }
        onStatusChanged: {
            if (loader.status == Loader.Ready && loader.source !== "") {
                if (loader.item.implicitWidth === 0 || loader.item.implicitHeight === 0) {
                    print("Implicit size is 0, using root size");
                    loader.item.implicitWidth = root.width;
                    loader.item.implicitHeight = root.height;
                }

                // Resize to loaded widget size
                // Note: We must use implicit* here to not
                // break the set values.
                root.width = loader.item.implicitWidth;
                root.height = loader.item.implicitHeight;
                Widget.show();
                root.state = "in";
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
                webView.url = Qt.resolvedUrl(Widget.sourcePath);
            }
        }
    }
    Item {
        id: nav
        height: 20
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        Rectangle {
            id: bg
            anchors.fill: parent
            color: "black"
            opacity: 0
        }
        MouseArea {
            id: mouseArea
            height: 20
            property var clickPos

            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: true
            onEntered: imgClose.state = "areaHover"
            onExited: {
                if (mouseAreaClose.containsMouse)
                    return;
                imgClose.state = "";
            }

            onPressed: function (mouse) {
                clickPos = {
                    "x": mouse.x,
                    "y": mouse.y
                };
            }
            onPositionChanged: {
                if (mouseArea.pressed)
                    Widget.setPos(Widget.cursorPos().x - clickPos.x, Widget.cursorPos().y - clickPos.y);
            }
        }

        MouseArea {
            id: mouseAreaClose
            width: 16
            height: width
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onEntered: imgClose.state = "iconHover"
            onExited: imgClose.state = ""
            onClicked: {
                if (Qt.platform.os === "windows")
                    Widget.setWindowBlur(0);
                animFadeOut.start();
            }

            anchors {
                margins: 1
                top: parent.top
                right: parent.right
            }

            ColorImage {
                id: imgClose
                source: "qrc:/qml/ScreenPlayWidget/assets/icons/baseline-close-24px.svg"
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                sourceSize: Qt.size(width, height)
                opacity: 0
                color: "white"

                states: [
                    State {
                        name: "areaHover"
                        PropertyChanges {
                            target: imgClose
                            opacity: .5
                        }
                        
                        PropertyChanges {
                            target: bg
                            opacity: .3
                        }
                    },
                    State {
                        name: "iconHover"
                        PropertyChanges {
                            target: imgClose
                            opacity: 1
                        }
                    }
                ]
                transitions: [
                    Transition {
                        PropertyAnimation {
                            duration: 250
                            target: imgClose
                            property: "opacity"
                        }
                    }
                ]
            }
        }
    }

    states: [
        State {
            name: "in"
            PropertyChanges {
                target: root
                opacity: 1
            }
            PropertyChanges {
                target: bgNoise
                opacity: 0.05
            }
            PropertyChanges {
                target: loader
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"

            PropertyAnimation {
                targets: [root, bgNoise, loader, bg]
                duration: 250
                property: "opacity"
            }
        }
    ]
}
