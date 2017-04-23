import QtQuick 2.7

Image {
    id: image
    width: 320
    height: 121
    state: "loading"

    property string sourceImage: ""

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



    onSourceImageChanged: {
        image.source = sourceImage
    }

    opacity: 0
    anchors.bottomMargin: 0
    antialiasing: false
    asynchronous: true
    fillMode: Image.PreserveAspectCrop
    source: sourceImage
    anchors.fill: parent

    onStatusChanged: {
        if (image.status == Image.Ready)
            image.state = "loaded"
    }
}
