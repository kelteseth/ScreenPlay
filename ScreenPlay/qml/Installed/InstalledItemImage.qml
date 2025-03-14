import QtQuick
import ScreenPlayCore as Util

Item {
    id: root

    property string absoluteStoragePath
    property string sourceImage
    property string sourceImageGIF
    property int type

    function enter() {
        if (root.sourceImageGIF != "")
            loader_imgGIFPreview.sourceComponent = component_imgGIFPreview;
    }

    function exit() {
        root.state = "loaded";
        loader_imgGIFPreview.sourceComponent = null;
    }

    width: 320
    height: 121
    state: "loading"

    Image {
        id: image

        anchors.fill: parent
        asynchronous: true
        cache: true
        fillMode: Image.PreserveAspectCrop
        source: {
            if (root.sourceImage === "")
                return "qrc:/qt/qml/ScreenPlay/assets/images/missingPreview.png";
            return root.absoluteStoragePath + "/" + root.sourceImage;
        }
        onStatusChanged: {
            if (image.status === Image.Ready) {
                root.state = "loaded";
            } else if (image.status === Image.Error) {
                source = "qrc:/qt/qml/ScreenPlay/assets/images/missingPreview.png";
                root.state = "loaded";
            }
        }
    }

    Component {
        id: component_imgGIFPreview

        AnimatedImage {
            id: imgGIFPreview

            asynchronous: true
            playing: true
            source: root.sourceImageGIF === "" ? "qrc:/qt/qml/ScreenPlay/assets/images/missingPreview.png" : Qt.resolvedUrl(root.absoluteStoragePath + "/" + root.sourceImageGIF)
            fillMode: Image.PreserveAspectCrop
        }
    }

    Loader {
        id: loader_imgGIFPreview

        anchors.fill: parent
        opacity: 0
    }

    transitions: [
        Transition {
            from: "loading"
            to: "loaded"

            OpacityAnimator {
                target: image
                duration: 300
                from: 0
                to: 1
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "hover"
            to: "loaded"

            OpacityAnimator {
                target: loader_imgGIFPreview
                duration: 300
                from: 1
                to: 0
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "loaded"
            to: "hover"
            reversible: true

            OpacityAnimator {
                target: loader_imgGIFPreview
                duration: 400
                from: 0
                to: 1
                easing.type: Easing.OutQuart
            }
        }
    ]
}
