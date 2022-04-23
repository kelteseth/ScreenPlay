import QtQuick
import QtQuick.Controls
import QtQuick.Layouts 
import QtQuick.Controls.Material
import QtQuick.Particles
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0
import ScreenPlayUtil 1.0 as Common

Item {
    id: delegate

    property alias image: image.source
    property alias category: txtCategory.text
    property alias text: txtText.text
    property alias description: description.text
    property url link

    Rectangle {
        id: img

        anchors.fill: parent
        anchors.margins: 5
        clip: true
        layer.enabled: true

        Image {
            id: image

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
        }

        LinearGradient {
            anchors.fill: parent
            end: Qt.point(0, 0)
            start: Qt.point(0, parent.height * 0.66)

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#DD000000"
                }

                GradientStop {
                    position: 1
                    color: "#00000000"
                }

            }

        }

        Text {
            id: txtCategory

            font.pointSize: 10
            font.family: ScreenPlay.settings.font
            color: "white"

            anchors {
                left: parent.left
                right: parent.right
                bottom: txtText.top
                margins: 15
                bottomMargin: 5
            }

        }

        Text {
            id: txtText

            font.pointSize: 16
            font.family: ScreenPlay.settings.font
            color: "white"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 15
            }

        }

        Rectangle {
            color: Material.backgroundDimColor

            anchors {
                top: img.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            Text {
                id: description

                font.pointSize: 14
                font.family: ScreenPlay.settings.font
                color: Material.primaryTextColor

                anchors {
                    fill: parent
                    margins: 5
                }

            }

        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: Qt.openUrlExternally(delegate.link)
            onEntered: delegate.state = "hover"
            onExited: delegate.state = ""
        }

        layer.effect: ElevationEffect {
            elevation: 4
        }

    }

    transitions: [
        Transition {
            from: ""
            to: "hover"

            ScaleAnimator {
                target: image
                duration: 80
                from: 1
                to: 1.05
            }

        },
        Transition {
            from: "hover"
            to: ""

            ScaleAnimator {
                target: image
                duration: 80
                from: 1.05
                to: 1
            }

        }
    ]
}
