import QtQuick 2.13
import QtGraphicalEffects 1.0
import ScreenPlay.Workshop 1.0

Rectangle {
    id: root
    color: "#161C1D"
    property string backgroundImage: ""
    property int imageOffsetTop: 0
    onImageOffsetTopChanged: {
        if ((imageOffsetTop * -1) >= 200) {
            root.state = "backgroundColor"
        } else {
            if (root.state !== "backgroundImage") {
                root.state = "backgroundImage"
            }
        }
    }
    onBackgroundImageChanged: {
        if (backgroundImage === "") {
            root.state = ""
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
                    position: 0.6
                    color: "#00ffffff"
                }
                GradientStop {
                    position: 1
                    color: "#161C1D"
                }
            }
        }
    }

    MaskedBlur {
        id: blur
        anchors.fill: bgImage
        source: bgImage
        maskSource: maskSource
        radius: 16
        cached: true
        samples: 24
        opacity: 0
    }

    Rectangle {
        id: bgColor
        color: "#161C1D"
        opacity: 0
        anchors.fill: parent
    }

    states: [
        State {
            name: ""
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
                opacity: 0
            }
        },
        State {
            name: "backgroundColor"
            PropertyChanges {
                target: bgImage
                opacity: 1
            }

            PropertyChanges {
                target: bgColor
                opacity: 0.8
            }
            PropertyChanges {
                target: blur
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "backgroundImage"
            reversible: true
            PropertyAnimation {
                targets: [bgImage, bgColor, blur]
                duration: 500
                easing.type: Easing.InOutQuart
                property: "opacity"
            }
        },
        Transition {
            from: "backgroundImage"
            to: "backgroundColor"
            reversible: true
            PropertyAnimation {
                targets: [bgImage, bgColor, blur]
                duration: 200
                easing.type: Easing.InOutQuart
                property: "opacity"
            }
        }
    ]
}
