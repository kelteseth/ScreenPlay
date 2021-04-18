import QtQml 2.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import ScreenPlayWallpaper 1.0
import ScreenPlay.Enums.InstalledType 1.0
import ScreenPlay.Shader 1.0
import "ShaderWrapper" as ShaderWrapper

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
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
        }

        // Replace wallpaper with QML Scene
        function onReloadQML(oldType) {

            loader.sourceComponent = undefined
            loader.source = ""
            Wallpaper.clearComponentCache()

            loader.source = Qt.resolvedUrl(Wallpaper.fullContentPath)
        }

        // Replace wallpaper with GIF
        function onReloadGIF(oldType) {
            init()
        }

        // This function only gets called here (the same function
        // is inside the WebView.qml) when the previous Wallpaper type
        // was not a video
        function onReloadVideo(oldType) {
            // We need to check if the old type
            // was also Video not get called twice
            if (oldType === InstalledType.VideoWallpaper)
                return

            imgCover.state = "in"
            loader.source = "qrc:/WebView.qml"
        }
    }

    function init() {
        switch (Wallpaper.type) {
        case InstalledType.VideoWallpaper:
            loader.source = "qrc:/WebView.qml"
            break
        case InstalledType.HTMLWallpaper:
            loader.setSource("qrc:/WebView.qml", {
                                 "url": Qt.resolvedUrl(
                                            Wallpaper.fullContentPath)
                             })
            break
        case InstalledType.QMLWallpaper:
            loader.source = Qt.resolvedUrl(Wallpaper.fullContentPath)
            break
        case InstalledType.WebsiteWallpaper:
            loader.setSource("qrc:/WebsiteWallpaper.qml", {
                                 "url": Wallpaper.fullContentPath
                             })
            break
        case InstalledType.GifWallpaper:
            loader.setSource("qrc:/GifWallpaper.qml", {
                                 "source": Qt.resolvedUrl(
                                               Wallpaper.fullContentPath)
                             })
            break
        }

        fadeIn()
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
        // QML Engine deadlocks in 5.15.2 when a loader cannot load
        // an item. QApplication::quit(); waits for the destruction forever.
        asynchronous: true
        onStatusChanged: {
            if (loader.status === Loader.Error) {
                loader.source = ""
                Wallpaper.terminate()
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
            topMargin: -3 // To fix the offset from setupWallpaperForOneScreen
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

            if (Qt.platform.os !== "windows") {
                root.canFadeByWallpaperFillMode = false
                return
            }

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
                root.canFadeByWallpaperFillMode = false
                break
            }
        }
    }

    Pane {
        id: debug
        visible: Wallpaper.debugMode
        enabled: Wallpaper.debugMode
        width: parent.width * .3
        height: parent.height * .3
        anchors.centerIn: parent
        background: Rectangle {
            opacity: .5
        }

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10
            Text {
                text: "appID " + Wallpaper.appID
                font.pointSize: 14
            }
            Text {
                text: "basePath " + Wallpaper.basePath
                font.pointSize: 14
            }
            Text {
                text: "fullContentPath " + Wallpaper.fullContentPath
                font.pointSize: 14
            }
            Text {
                text: "fillMode " + Wallpaper.fillMode
                font.pointSize: 14
            }
            Text {
                text: "sdk.type " + Wallpaper.sdk.type
                font.pointSize: 14
            }
            Text {
                text: "sdk.isConnected " + Wallpaper.sdk.isConnected
                font.pointSize: 14
            }
            Text {
                text: "sdk.appID " + Wallpaper.sdk.appID
                font.pointSize: 14
            }
        }
    }
}
