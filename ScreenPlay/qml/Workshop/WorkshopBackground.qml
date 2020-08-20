import QtQuick 2.13
import QtGraphicalEffects 1.0
import ScreenPlay.Workshop 1.0

Rectangle {
    id: root
    state: "base"
    color: "#161C1D"
    property string backgroundImage: ""
    property int imageOffsetTop: 0
    onImageOffsetTopChanged:  {
        if ((imageOffsetTop * -1) >= 300) {
            root.state = "backgroundColor"
        } else {
            if (root.state !== "backgroundImage") {
                root.state = "backgroundImage"
            }
        }
    }
    onBackgroundImageChanged: {
        if (backgroundImage === "") {
            root.state = "base"
        } else {
            root.state = "backgroundImage"
        }
    }

    Image {
        id: maskSource
        visible: false
        source: "qrc:/assets/images/mask_workshop.png"
    }




    Image {
        id: bgImage
        height: bgImage.sourceSize.height
        anchors {
            topMargin: root.imageOffsetTop
            top: parent.top
            right: parent.right
            left: parent.left
        }

        fillMode: Image.PreserveAspectCrop
        opacity: 0
        source: root.backgroundImage

        LinearGradient {
            id: gradient
            anchors.fill: parent
            z: 4
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#00ffffff"
                }
                GradientStop {
                    position: .9
                    color: "#161C1D"
                }
            }
        }
    }

    MaskedBlur {
        id:blur
        anchors.fill: bgImage
        source: bgImage
        maskSource: maskSource
        radius: 16
        cached: true
        samples: 24
    }



    Rectangle {
        id: bgColor
        color: "#161C1D"
        opacity: 0
        anchors.fill: parent
    }


    states: [
        State {
            name: "base"
            PropertyChanges {
                target: bgImage
                opacity: 0
            }

            PropertyChanges {
                target: bgColor
                opacity: 0
            }
            PropertyChanges {
                target: blur
                opacity: 0
            }
        },
        State {
            name: "backgroundImage"
            PropertyChanges {
                target: bgImage
                opacity: 1
            }

            PropertyChanges {
                target: bgColor
                opacity: 0
            }
            PropertyChanges {
                target: blur
                opacity: 1
            }
        },
        State {
            name: "backgroundColor"
            PropertyChanges {
                target: bgImage
                opacity: 0
            }

            PropertyChanges {
                target: bgColor
                opacity: 1
            }
            PropertyChanges {
                target: blur
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "base"
            to: "backgroundImage"
            reversible: true
            PropertyAnimation {
                targets: [bgImage,  bgColor, blur]
                duration: 2000
                easing.type: Easing.InOutQuart
                property: "opacity"
            }
        },
        Transition {
            from: "backgroundImage"
            to: "backgroundColor"
            reversible: true
            PropertyAnimation {
                targets: [bgImage,  bgColor]
                duration: 1000
                easing.type: Easing.InOutQuart
                property: "opacity"
            }
        }
    ]
}
