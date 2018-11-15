import QtQuick 2.9

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

            PropertyChanges {
                target: text1
                wrapMode: Text.WrapAnywhere
            }
        },
        State {
            name: "loaded"

            PropertyChanges {
                target: image
                opacity: 1
            }

            PropertyChanges {
                target: text1
                opacity: 0
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
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: text1
                property: "opacity"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
