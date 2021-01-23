import QtQuick 2.12
import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0

Item {
    id: root
    width: 320
    height: 121
    state: "loading"

    property string absoluteStoragePath
    property string sourceImage
    property string sourceImageGIF
    property var type: InstalledType.Unknown
    onTypeChanged: {
        if (root.sourceImage === "" && root.sourceImageGIF === "") {
            image.source = "qrc:/assets/images/missingPreview.png"
            return
        }

        if (root.type === InstalledType.GifWallpaper) {
            image.source = Qt.resolvedUrl(
                        absoluteStoragePath + "/" + root.sourceImageGIF)
            print(image.source)
        } else {
            if (root.sourceImage !== "") {
                image.source = Qt.resolvedUrl(
                            absoluteStoragePath + "/" + root.sourceImage)
            }
        }
    }

    function enter() {
        if (root.type !== InstalledType.GifWallpaper) {
            if (root.sourceImageGIF !== "")
                image.source = Qt.resolvedUrl(
                            absoluteStoragePath + "/" + root.sourceImageGIF)
        }
        image.playing = true
    }

    function exit() {
        image.playing = false
        if (root.type !== InstalledType.GifWallpaper) {
            image.source = Qt.resolvedUrl(
                        absoluteStoragePath + "/" + root.sourceImage)
        }
    }

    AnimatedImage {
        id: image
        anchors.fill: parent
        asynchronous: true
        cache: true
        playing: false
        fillMode: Image.PreserveAspectCrop

        onStatusChanged: {
            if (image.status === Image.Ready) {
                root.state = "loaded"
            } else if (image.status === Image.Error) {
                source = "qrc:/assets/images/missingPreview.png"
                root.state = "loaded"
            }
        }
    }
}
