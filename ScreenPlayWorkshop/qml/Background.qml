import QtQuick
import QtQuick.Effects

Rectangle {
    id: root

    /*! Exposes the stable committed image for external ShaderEffectSource
        consumers such as the ImageBlurContainer. */
    property alias image: bgCurrent.imageItem

    property string backgroundImage: ""
    property int imageOffsetTop: 0
    property int stackViewDepth: 0

    /*! True while a crossfade transition between two background images is
        running. Bind ImageBlurContainer.backgroundTransitioning to this so
        the blur stays up-to-date during the animation. */
    readonly property bool transitioning: transitionAnim.running

    onStackViewDepthChanged: {
        root.state = stackViewDepth > 1 ? "backgroundBlur" : "backgroundImage"
    }

    color: "#161C1D"
    onImageOffsetTopChanged: {
        if ((imageOffsetTop * -1) >= 200) {
            root.state = "backgroundColor"
        } else {
            if (root.state !== "backgroundImage")
                root.state = "backgroundImage"
        }
    }

    onBackgroundImageChanged: {
        if (backgroundImage === "") {
            transitionAnim.stop()
            bgNext.source = ""
            bgNext.opacity = 0
            bgCurrent.source = ""
            bgCurrent.opacity = 0
            root.state = ""
            return
        }
        bgNext.source = backgroundImage
        transitionAnim.restart()
        if (root.state === "")
            root.state = "backgroundImage"
    }

    /*! \brief Pure visual container: a single image with a gradient overlay.
        All transition logic lives in Background.qml which manages two
        instances (bgCurrent / bgNext) and switches between them. */
    component FadeImage: Item {
        id: fadeRoot

        property string source: ""
        // Exposes the inner Image for MultiEffect sourcing.
        property alias imageItem: img
        required property int offsetTop

        Image {
            id: img

            height: img.sourceSize.height
            source: fadeRoot.source

            anchors {
                top: parent.top
                topMargin: fadeRoot.offsetTop
                left: parent.left
                right: parent.right
            }

            Rectangle {
                anchors.fill: parent
                z: 4

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#00161C1D"
                    }

                    GradientStop {
                        position: 0.4
                        color: "#00161C1D"
                    }

                    GradientStop {
                        position: 0.75
                        color: "#80161C1D"
                    }

                    GradientStop {
                        position: 1
                        color: "#161C1D"
                    }
                }
            }
        }
    }

    Image {
        id: maskSource

        visible: false
        source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/images/mask_workshop.png"
    }

    // Stable committed layer. Always holds the last fully-transitioned image.
    FadeImage {
        id: bgCurrent

        offsetTop: root.imageOffsetTop
        opacity: 0
        anchors.fill: parent
    }

    // Incoming layer. Fades in over bgCurrent during a transition.
    FadeImage {
        id: bgNext

        offsetTop: root.imageOffsetTop
        opacity: 0
        z: 2
        anchors.fill: parent
    }

    // Persistent blur used by backgroundBlur / backgroundColor states.
    MultiEffect {
        id: blur

        anchors.fill: bgCurrent
        source: bgCurrent.imageItem
        blurEnabled: true
        blurMax: 64
        blur: 0.25
        autoPaddingEnabled: true
        opacity: 0
        z: 4
    }

    // Dark tint overlay for backgroundColor / backgroundBlur states.
    Rectangle {
        id: bgColor

        color: "#161C1D"
        opacity: 0
        anchors.fill: parent
        z: 5
    }

    ParallelAnimation {
        id: transitionAnim

        OpacityAnimator {
            target: bgCurrent
            from: bgCurrent.opacity
            to: 0
            duration: 400
        }

        OpacityAnimator {
            target: bgNext
            from: 0
            to: 1
            duration: 400
        }

        onFinished: {
            bgCurrent.source = bgNext.source
            bgCurrent.opacity = 1
            bgNext.opacity = 0
            bgNext.source = ""
        }
    }

    states: [
        State {
            name: ""

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
                target: bgColor
                opacity: 0
            }

            PropertyChanges {
                target: blur
                opacity: 0
                blur: 0.25
            }
        },
        State {
            name: "backgroundColor"

            PropertyChanges {
                target: bgColor
                opacity: 0.4
            }

            PropertyChanges {
                target: blur
                opacity: 1
                blur: 0.25
            }
        },
        State {
            name: "backgroundBlur"

            PropertyChanges {
                target: bgColor
                opacity: 0.5
            }

            PropertyChanges {
                target: blur
                opacity: 1
                blur: 1.0
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "backgroundImage"
            reversible: true

            PropertyAnimation {
                targets: [bgColor, blur]
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
                targets: [bgColor, blur]
                duration: 600
                easing.type: Easing.InOutQuad
                property: "opacity"
            }
        },
        Transition {
            from: "backgroundImage"
            to: "backgroundBlur"
            reversible: true

            PropertyAnimation {
                targets: [bgColor, blur]
                duration: 120
                easing.type: Easing.InOutQuart
                property: "opacity"
            }

            PropertyAnimation {
                target: blur
                duration: 120
                easing.type: Easing.InOutQuart
                property: "blur"
            }
        }
    ]
}
