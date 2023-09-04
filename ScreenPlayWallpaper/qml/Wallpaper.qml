import QtQml
import QtQuick
import QtQuick.Controls
import ScreenPlayWallpaper
import ScreenPlay.Enums.InstalledType
import ScreenPlay.Enums.VideoCodec

Rectangle {
    id: root

    property bool canFadeByWallpaperFillMode: true
    anchors.fill: parent

    function start() {
        fadeInImageSetup();
        switch (Wallpaper.type) {
        case InstalledType.VideoWallpaper:
            if (Wallpaper.videoCodec === VideoCodec.Unknown) {
                Wallpaper.terminate();
            }

            // macOS only supports h264 via the native Qt MM
            if (Qt.platform.os === "osx") {
                if ((Wallpaper.videoCodec === VideoCodec.VP8 || Wallpaper.videoCodec === VideoCodec.VP9)) {
                    loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaWebView.qml";
                } else {
                    loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaView.qml";
                }
            }
            if (Qt.platform.os === "windows") {
                loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaView.qml";
            }
            break;
        case InstalledType.HTMLWallpaper:
            loader.setSource("qrc:/qml/ScreenPlayWallpaper/qml/WebsiteWallpaper.qml", {
                    "url": Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
                });
            break;
        case InstalledType.QMLWallpaper:
            loader.source = Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute);
            break;
        case InstalledType.WebsiteWallpaper:
            loader.setSource("qrc:/qml/ScreenPlayWallpaper/qml/WebsiteWallpaper.qml", {
                    "url": Wallpaper.projectSourceFileAbsolute
                });
            break;
        case InstalledType.GifWallpaper:
            loader.setSource("qrc:/qml/ScreenPlayWallpaper/qml/GifWallpaper.qml", {
                    "source": Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
                });
            break;
        }
    }

    function fadeInImageSetup() {
        if (Qt.platform.os !== "windows") {
            root.canFadeByWallpaperFillMode = false;
            return;
        }
        imgCover.source = Qt.resolvedUrl("file:///" + Wallpaper.windowsDesktopProperties.wallpaperPath);
        switch (Wallpaper.windowsDesktopProperties.wallpaperStyle) {
        case 10:
            imgCover.fillMode = Image.PreserveAspectCrop;
            // We only support fade in for one screen
            if (Wallpaper.activeScreensList.length !== 1)
                return;
            if (Wallpaper.width === 0)
                return;
            if (Wallpaper.width > Wallpaper.windowsDesktopProperties.defaultWallpaperSize.width)
                return;

            // Windows does some weird top margin if the Wallpaper
            // is bigger than the monitor. So instead of centering
            // it vertically it moves the wallpaper down by 1/3 of
            // the remaining height.
            // 1. Scale down the wallpaper based on the height.
            // The default Windows 11 wallpaper C:\Windows\Web\Wallpaper\Windows\img19.jpg
            // has a resoltion of 3841x2400 scaled down to 3440x2150,3 with a given monitor
            // resolution of 3440x1440 resulting in a 2150,3 - 1440 = 710,3
            // 710,3 / (1/3) = 236,767
            const monitorWidth = Wallpaper.width;
            const monitorHeight = Wallpaper.height;
            const windowsWallpaperWidth = Wallpaper.windowsDesktopProperties.defaultWallpaperSize.width;
            const windowsWallpapeHeight = Wallpaper.windowsDesktopProperties.defaultWallpaperSize.height;

            // 1. Get scale factor:
            //    ->  3440 / 3840 = 0.8956
            const scaleFactor = monitorWidth / windowsWallpaperWidth;
            // 2. Scale down the default Windows wallpaper height (width stays the same for correct aspect ratio):
            //    -> 2400 * 0.8956 = 2149.4
            const scaledDownDefaultWallpaperHeight = windowsWallpapeHeight * scaleFactor;
            // 3. Calc offste
            //    -> 2150,3 - 1440 = 710,3
            const offset = scaledDownDefaultWallpaperHeight - monitorHeight;
            // 4. Calc the one third offset (topMargin)
            //    -> 710,3 * (1/3) = 236,767
            const topMargin = Math.floor(offset * 0.3333333);
            imgCover.anchors.topMargin = -topMargin;
            break;
        case 6:
            imgCover.fillMode = Image.PreserveAspectFit;
            break;
        case 2:
            break;
        case 0:
            if (Wallpaper.windowsDesktopProperties.isTiled) {
                // Tiled
                imgCover.fillMode = Image.Tile;
            } else {
                // Center
                imgCover.fillMode = Image.PreserveAspectFit;
                imgCover.anchors.centerIn = parent;
                imgCover.width = sourceSize.width;
                imgCover.height = sourceSize.height;
            }
            break;
        case 22:
            root.canFadeByWallpaperFillMode = false;
            break;
        }
        // NOTE: If we do not set it visible here
        // AND in the fadeIn function we get a white frame
        Wallpaper.setVisible(true);
    }

    function fadeIn() {
        Wallpaper.setVisible(true);
        if (canFadeByWallpaperFillMode && Wallpaper.canFade)
            imgCover.state = "hideDefaultBackgroundImage";
        else
            imgCover.opacity = 0;
    }

    Connections {
        target: Wallpaper
        function onQmlStart() {
            root.start();
        }
        function onFadeIn() {
            root.fadeIn();
        }
        function onQmlExit() {
            if (canFadeByWallpaperFillMode && Wallpaper.canFade)
                imgCover.state = "exit";
            else
                Wallpaper.terminate();
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
            Wallpaper.clearComponentCache();
            loader.source = Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute);
        }

        // Replace wallpaper with GIF
        function onReloadGIF(oldType) {
            init();
        }

        // This function only gets called here (the same function
        // is inside the MultimediaWebView.qml) when the previous Wallpaper type
        // was not a video
        function onReloadVideo(oldType) {
            // We need to check if the old type
            // was also Video not get called twice
            if (oldType === InstalledType.VideoWallpaper)
                return;
            loader.source = "qrc:/qml/ScreenPlayWallpaper/qml/MultimediaView.qml";
        }
    }
    Loader {
        id: loader
        anchors.fill: parent
        // QML Engine deadlocks in 5.15.2 when a loader cannot load
        // an item. QGuiApplication::quit(); waits for the destruction forever.
        //asynchronous: true
        onStatusChanged: {
            if (loader.status === Loader.Ready) {
                if (Wallpaper.type === InstalledType.QMLWallpaper) {
                    root.fadeIn();
                }
            }
            if (loader.status === Loader.Error) {
                print("ScreenPlayWallpaper encountered an error and will be terminated.");
                // Must be callLater so we do not kill on startup
                // See  emit window.qmlStart();
                Qt.callLater(function () {
                        loader.source = "";
                        Qt.callLater(function () {
                                Wallpaper.terminate();
                            });
                    });
            }
        }
    }

    Image {
        id: imgCover
        state: "showDefaultBackgroundImage"
        sourceSize.width: Wallpaper.width
        sourceSize.height: Wallpaper.height

        anchors {
            top: parent.top
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

    FrameAnimation {
        id: frameAnimation
        property real fps: smoothFrameTime > 0 ? (1.0 / smoothFrameTime) : 0
        readonly property string ft: (1000 * smoothFrameTime).toFixed(1)
        readonly property real maxFPS: 24.0
        readonly property real maxPeriod: 1.0 / maxFPS
        readonly property int skipFrames: fps / maxFPS
        running: Wallpaper.debugMode
    }

    Pane {
        id: debug

        visible: Wallpaper.debugMode
        enabled: Wallpaper.debugMode
        width: parent.width * 0.6
        height: parent.height * 0.5
        anchors.centerIn: parent

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10

            Text {
                visible: Wallpaper.debugMode
                enabled: Wallpaper.debugMode
                font.pointSize: 14
                text: "fps: " + frameAnimation.fps.toFixed(0) + " - Frame time:" + frameAnimation.ft + " ms"
            }

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
                text: "projectSourceFileAbsolute " + Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
                font.pointSize: 14
            }

            Text {
                text: "fillMode " + Wallpaper.fillMode
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
                        return "imgCover.source " + Qt.resolvedUrl("file:///" + Wallpaper.windowsDesktopProperties.wallpaperPath);
                    else
                        return "";
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
