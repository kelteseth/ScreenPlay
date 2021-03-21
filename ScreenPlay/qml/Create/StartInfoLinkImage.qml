import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

import "../Common" as Common

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
        layer.effect: ElevationEffect {
            elevation: 4
        }

        Image {
            id: image
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
        }

        LinearGradient {
            anchors.fill: parent
            end: Qt.point(0, 0)
            start: Qt.point(0, parent.height * .66)
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#DD000000"
                }
                GradientStop {
                    position: 1.0
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
