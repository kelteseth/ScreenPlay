import QtQuick 2.12

Item {
    id: screenPlayItemImage
    width: 320
    height: 121
    state: "loading"

    property string sourceImage: ""
    property string sourceImageGIF: ""

    function enter() {
        if (screenPlayItemImage.sourceImageGIF != "") {
            imgGIFPreview.playing = true
        }
    }

    function exit() {
        imgGIFPreview.playing = false
    }

    Image {
        id: image
        anchors.fill: parent
        antialiasing: false
        asynchronous: true
        fillMode: Image.PreserveAspectCrop
        source: screenPlayItemImage.sourceImage.trim()

        onStatusChanged: {
            if (image.status === Image.Ready) {
                screenPlayItemImage.state = "loaded"
            } else if (image.status === Image.Error) {
                source = "qrc:/assets/images/missingPreview.png"
                screenPlayItemImage.state = "loaded"
            }
        }
    }

    AnimatedImage {
        id: imgGIFPreview
        asynchronous: true
        playing: false
        opacity: 0
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: screenPlayItemImage.sourceImageGIF.trim()
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
                target: imgGIFPreview
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
                target: imgGIFPreview
                property: "opacity"
                duration: 400
                easing.type: Easing.OutQuart
            }
        }
    ]
}
