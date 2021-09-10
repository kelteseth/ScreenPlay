import QtQuick

Item {
    id: screenPlayItemImage

    property string sourceImage
    property string sourceImageGIF

    width: 320
    height: 121
    state: "loading"

    Image {
        id: image

        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: screenPlayItemImage.sourceImage.trim()
        onStatusChanged: {
            if (image.status === Image.Ready) {
                screenPlayItemImage.state = "loaded";
            } else if (image.status === Image.Error) {
                source = "images/missingPreview.png";
                screenPlayItemImage.state = "loaded";
            }
        }
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

        }
    ]
}
