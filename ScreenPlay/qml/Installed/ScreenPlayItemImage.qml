import QtQuick 2.12

Item {
    id: root
    width: 320
    height: 121
    state: "loading"

    property string absoluteStoragePath
    property string sourceImage
    property string sourceImageGIF

    function enter() {
        if (root.sourceImageGIF != "") {
            loader_imgGIFPreview.sourceComponent = component_imgGIFPreview
        }
    }

    function exit() {
        root.state = "loaded"
        loader_imgGIFPreview.sourceComponent = null
    }

    Image {
        id: image
        anchors.fill: parent
        asynchronous: true
        cache: true
        fillMode: Image.PreserveAspectCrop
        source: root.screenPreview
                === "" ? "qrc:/assets/images/missingPreview.png" : Qt.resolvedUrl(
                             absoluteStoragePath + "/" + root.sourceImage)

        onStatusChanged: {
            if (image.status === Image.Ready) {
                root.state = "loaded"
            } else if (image.status === Image.Error) {
                source = "qrc:/assets/images/missingPreview.png"
                root.state = "loaded"
            }
        }
    }

    Component {
        id: component_imgGIFPreview
        AnimatedImage {
            id: imgGIFPreview
            asynchronous: true
            playing: true
            source: root.sourceImageGIF
                    === "" ? "qrc:/assets/images/missingPreview.png" : Qt.resolvedUrl(
                                 absoluteStoragePath + "/" + root.sourceImageGIF)
            fillMode: Image.PreserveAspectCrop
        }
    }
    Loader {
        id: loader_imgGIFPreview
        anchors.fill: parent
        opacity: 0
    }

    states: [
        State {
            name: "loading"

            PropertyChanges {
                target: image
                opacity: 0
            }
        },
        State {
            name: "loaded"

            PropertyChanges {
                target: image
                opacity: 1
            }
        },
        State {
            name: "hover"

            PropertyChanges {
                target: loader_imgGIFPreview
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: "loading"
            to: "loaded"

            NumberAnimation {
                target: image
                property: "opacity"
                duration: 300
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "loaded"
            to: "hover"
            reversible: true

            PropertyAnimation {
                target: loader_imgGIFPreview
                property: "opacity"
                duration: 400
                easing.type: Easing.OutQuart
            }
        }
    ]
}
