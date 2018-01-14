import QtQuick 2.9

Item {
    id: screenPlayItemImage
    width: 320
    height: 121
    anchors.bottomMargin: 0
    state: "loading"

    property string sourceImage: ""

    onSourceImageChanged: {
        image.source = screenPlayItemImage.sourceImage
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


    Image {
        id:image
        anchors.fill: parent
        antialiasing: false
        asynchronous: true

        //sourceSize: Qt.size(320,121)
        fillMode: Image.PreserveAspectCrop
        source: screenPlayItemImage.sourceImage.trim()

        onStatusChanged: {

            if (image.status === Image.Ready){
                screenPlayItemImage.state = "loaded"
            } else if(image.status === Image.Error){
                source = "qrc:/assets/images/missingPreview.png"
                screenPlayItemImage.state = "loaded"
            }

        }
    }

    Text {
        id: text1
        text: ""
        anchors.fill: parent
        font.pixelSize: 12
    }
}
