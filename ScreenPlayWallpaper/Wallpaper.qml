import QtQuick 2.14
import QtQml 2.14
import ScreenPlayWallpaper 1.0

Rectangle {
    id: root
    anchors.fill: parent
    color: {
        if (Qt.platform.os !== "windows") {
            return "black"
        } else {
            return Wallpaper.windowsDesktopProperties.color
        }
    }

    property bool canFadeByWallpaperFillMode: true

    Component.onCompleted: {
        init()
    }

    Connections {
        target: Wallpaper

        function onQmlExit() {
            if (canFadeByWallpaperFillMode && Wallpaper.canFade) {
                imgCover.state = "outExit"
            } else {
                Wallpaper.terminate()
            }
        }

        function onQmlSceneValueReceived(key, value) {
            var obj2 = 'import QtQuick 2.0; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
            print(key, value)
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
        }

        function onReloadQML() {
            loader.sourceComponent = undefined
            loader.source = ""
            Wallpaper.clearComponentCache()
            root.init()
        }
    }

    function init() {
        switch (Wallpaper.type) {
        case Wallpaper.WallpaperType.Video:
            loader.source = "qrc:/WebView.qml"
            break
        case Wallpaper.WallpaperType.Html:
            loader.webViewUrl = Qt.resolvedUrl(Wallpaper.fullContentPath)
            loader.source = "qrc:/WebView.qml"
             fadeIn()
            break
        case Wallpaper.WallpaperType.Qml:
            loader.source = Qt.resolvedUrl(Wallpaper.fullContentPath)
            imgCover.state = "out"
             fadeIn()
            break
        }
    }

    function fadeIn() {
        Wallpaper.setVisible(true)
        if (canFadeByWallpaperFillMode && Wallpaper.canFade) {
            imgCover.state = "out"
        } else {
            imgCover.opacity = 0
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        property string webViewUrl
        onStatusChanged: {
            if (loader.status === Loader.Ready) {
                if (Wallpaper.type === Wallpaper.WallpaperType.Html
                        || Wallpaper.type === Wallpaper.WallpaperType.ThreeJSScene) {
                    loader.item.url = loader.webViewUrl
                    print(loader.item.url, " --- ", loader.webViewUrl)
                }
            }
        }

        Connections {
            ignoreUnknownSignals: true
            target: loader.item
            function onRequestFadeIn() {
                fadeIn()
            }
        }
    }

    Image {
        id: imgCover
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        state: "in"

        sourceSize.width: Wallpaper.width
        sourceSize.height: Wallpaper.height
        source: {
            if (Qt.platform.os === "windows") {
                return Qt.resolvedUrl(
                            "file:///" + Wallpaper.windowsDesktopProperties.wallpaperPath)
            } else {
                return ""
            }
        }

        states: [
            State {
                name: "in"
                PropertyChanges {
                    target: imgCover
                    opacity: 1
                }
            },
            State {
                name: "out"
                PropertyChanges {
                    target: imgCover
                    opacity: 0
                }
            },
            State {
                name: "outExit"
                PropertyChanges {
                    target: imgCover
                    opacity: 1
                }
            }
        ]
        transitions: [
            Transition {
                from: "out"
                to: "in"
                reversible: true
                PropertyAnimation {
                    target: imgCover
                    duration: 600
                    property: "opacity"
                }
            },
            Transition {
                from: "out"
                to: "outExit"
                reversible: true
                SequentialAnimation {
                    PropertyAnimation {
                        target: imgCover
                        duration: 600
                        property: "opacity"
                    }
                    ScriptAction {
                        script: Wallpaper.terminate()
                    }
                }
            }
        ]

        Component.onCompleted: {

            if (Qt.platform.os !== "windows")
                return

            switch (Wallpaper.windowsDesktopProperties.wallpaperStyle) {
            case 10:
                imgCover.fillMode = Image.PreserveAspectCrop
                break
            case 6:
                imgCover.fillMode = Image.PreserveAspectFit
                break
            case 2:
                break
            case 0:
                if (desktopProperties.isTiled) {
                    // Tiled
                    imgCover.fillMode = Image.Tile
                } else {
                    // Center
                    imgCover.fillMode = Image.PreserveAspectFit
                    imgCover.anchors.centerIn = parent
                    imgCover.width = sourceSize.width
                    imgCover.height = sourceSize.height
                }
                break
            case 22:
                canFadeByWallpaperFillMode = false
                break
            }
        }
    }
}
