import QtQml
import QtQuick
import QtQuick.Controls
import ScreenPlayWallpaper
import ScreenPlay.Enums.InstalledType
import ScreenPlay.Enums.VideoCodec

Rectangle {
    id: root
    onStateChanged: print(state)

    property bool canFadeByWallpaperFillMode: true

    function init() {
        switch (Wallpaper.type) {
        case InstalledType.VideoWallpaper:
            if (Wallpaper.videoCodec === VideoCodec.Unknown) {
                Wallpaper.terminate()
            }

            // macOS only supports h264 via the native Qt MM
            if (Qt.platform.os === "osx") {
                if ((Wallpaper.videoCodec === VideoCodec.VP8
                     || Wallpaper.videoCodec === VideoCodec.VP9)) {
                    loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaWebView.qml"
                } else {
                    loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaView.qml"
                }
            }

            if (Qt.platform.os === "windows") {
                loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaView.qml"
            }

            fadeIn()
            break
        case InstalledType.HTMLWallpaper:
            loader.setSource(
                        "qrc:/qml/ScreenPlayWallpaper/qml/WebsiteWallpaper.qml",
                        {
                            "url": Qt.resolvedUrl(
                                       Wallpaper.projectSourceFileAbsolute)
                        })
            break
        case InstalledType.QMLWallpaper:
            loader.source = Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
            fadeIn()
            break
        case InstalledType.WebsiteWallpaper:
            loader.setSource(
                        "qrc:/qml/ScreenPlayWallpaper/qml/WebsiteWallpaper.qml",
                        {
                            "url": Wallpaper.projectSourceFileAbsolute
                        })
            fadeIn()
            break
        case InstalledType.GifWallpaper:
            loader.setSource(
                        "qrc:/qml/ScreenPlayWallpaper/qml/GifWallpaper.qml", {
                            "source": Qt.resolvedUrl(
                                          Wallpaper.projectSourceFileAbsolute)
                        })
            fadeIn()
            break
        }
    }

    function fadeIn() {
        Wallpaper.setVisible(true)
        if (canFadeByWallpaperFillMode && Wallpaper.canFade)
            imgCover.state = "hideDefaultBackgroundImage"
        else
            imgCover.opacity = 0
    }

    anchors.fill: parent
    color: {
        if (Qt.platform.os !== "windows")
            return "black"
        else
            return Wallpaper.windowsDesktopProperties.color
    }
    Component.onCompleted: {
        init()
    }

    Connections {
        function onQmlExit() {
            if (canFadeByWallpaperFillMode && Wallpaper.canFade)
                imgCover.state = "exit"
            else
                Wallpaper.terminate()
        }

        function onQmlSceneValueReceived(key, value) {
            var obj2 = 'import QtQuick; Item {Component.onCompleted: loader.item.'
                    + key + ' = ' + value + '; }'
            var newObject = Qt.createQmlObject(obj2.toString(), root, "err")
            newObject.destroy(10000)
        }

        // Replace wallpaper with QML Scene
        function onReloadQML(oldType) {
            loader.sourceComponent = undefined
            loader.source = ""
            Wallpaper.clearComponentCache()
            loader.source = Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
        }

        // Replace wallpaper with GIF
        function onReloadGIF(oldType) {
            init()
        }

        // This function only gets called here (the same function
        // is inside the MultimediaWebView.qml) when the previous Wallpaper type
        // was not a video
        function onReloadVideo(oldType) {
            // We need to check if the old type
            // was also Video not get called twice
            if (oldType === InstalledType.VideoWallpaper)
                return

            loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaView.qml"
        }

        target: Wallpaper
    }

    Loader {
        id: loader

        anchors.fill: parent
        // QML Engine deadlocks in 5.15.2 when a loader cannot load
        // an item. QApplication::quit(); waits for the destruction forever.
        //asynchronous: true
        onStatusChanged: {
            if (loader.status === Loader.Error) {
                loader.source = ""
                // Wallpaper.terminate();
            }
        }

        Connections {
            function onRequestFadeIn() {
                fadeIn()
            }

            ignoreUnknownSignals: true
            target: loader.item
        }
    }

    Image {
        id: imgCover

        state: "showDefaultBackgroundImage"
        sourceSize.width: Wallpaper.width
        sourceSize.height: Wallpaper.height
        source: {
            if (Qt.platform.os === "windows")
                return Qt.resolvedUrl(
                            "file:///" + Wallpaper.windowsDesktopProperties.wallpaperPath)
            else
                return ""
        }

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

        anchors {
            top: parent.top
            // To fix the offset from setupWallpaperForOneScreen
            // but I'm not sure when it happens
            //topMargin: -3
            left: parent.left
            right: parent.right
        }

        states: [
            State {
                name: "showDefaultBackgroundImage"

                PropertyChanges {
                    target: imgCover
                    opacity: 1
                }
            },
            State {
                name: "hideDefaultBackgroundImage"

                PropertyChanges {
                    target: imgCover
                    opacity: 0
                }
            },
            State {
                name: "exit"

                PropertyChanges {
                    target: imgCover
                    opacity: 1
                }
            }
        ]
        transitions: [
            Transition {
                from: "showDefaultBackgroundImage"
                to: "hideDefaultBackgroundImage"
                reversible: true

                SequentialAnimation {
                    PauseAnimation {
                        duration: 100
                    }

                    PropertyAnimation {
                        target: imgCover
                        duration: 600
                        property: "opacity"
                    }
                }
            },
            Transition {
                from: "hideDefaultBackgroundImage"
                to: "exit"
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
    }

    Pane {
        id: debug

        visible: Wallpaper.debugMode
        enabled: Wallpaper.debugMode
        width: parent.width * 0.3
        height: parent.height * 0.3
        anchors.centerIn: parent

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10

            Text {
                text: "appID " + Wallpaper.appID
                font.pointSize: 14
            }

            Text {
                text: "projectPath " + Wallpaper.projectPath
                font.pointSize: 14
            }

            Text {
                text: "getApplicationPath " + Wallpaper.getApplicationPath()
                font.pointSize: 14
            }

            Text {
                text: "projectSourceFileAbsolute " + Qt.resolvedUrl(
                          Wallpaper.projectSourceFileAbsolute)
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

            Text {
                text: "canFadeByWallpaperFillMode " + canFadeByWallpaperFillMode
                font.pointSize: 14
            }

            Text {
                text: "Wallpaper.canFade " + Wallpaper.canFade
                font.pointSize: 14
            }

            Text {
                text: {
                    if (Qt.platform.os === "windows")
                        return "imgCover.source " + Qt.resolvedUrl(
                                    "file:///" + Wallpaper.windowsDesktopProperties.wallpaperPath)
                    else
                        return ""
                }
                font.pointSize: 14
            }

            Text {
                text: "imgCover.status " + imgCover.status
                font.pointSize: 14
            }

        }

        background: Rectangle {
            opacity: 0.5
        }
    }


}
