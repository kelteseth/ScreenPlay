import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0

import "../Installed"

Item {
    id: workshopItem
    width: 320
    height: 180

    property url imgUrl
    property string name
    property int steamID

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    RectangularGlow {
        id: effect
        anchors {
            top: parent.top
            topMargin: 3
        }

        height: parent.height
        width: parent.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Item {
        id: screenPlayItemWrapper
        anchors.centerIn: parent
        height: 180
        width: 320

        Image {
            id: mask
            source: "qrc:/assets/images/Window.svg"
            sourceSize: Qt.size(screenPlayItemWrapper.width,
                                screenPlayItemWrapper.height)
            visible: false
            smooth: true
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            antialiasing: true
        }

        Item {
            id: itemWrapper
            visible: false
            anchors {
                fill: parent
                margins: 5
            }

            ScreenPlayItemImage {
                id: screenPlayItemImage
                anchors.fill: parent
                sourceImage: workshopItem.imgUrl
            }

            LinearGradient {
                id: shadow
                height: 50
                opacity: 0

                cached: true

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                }
                start: Qt.point(0, 50)
                end: Qt.point(0, 0)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "#CC000000"
                    }
                    GradientStop {
                        position: 1.0
                        color: "#00000000"
                    }
                }
            }
            Text {
                id: txtTitle
                text: workshopItem.name
                renderType: Text.NativeRendering
                wrapMode: Text.WrapAnywhere
                opacity: 0
                height: 30
                width: 180
                verticalAlignment: Text.AlignVCenter
                color: "white"
                anchors {
                    bottom: parent.bottom
                    right: button.left
                    rightMargin: 10
                    left: parent.left
                    leftMargin: 20
                    bottomMargin: -50
                }
            }

            Button {
                id: button
                text: qsTr("Subscribe")
                anchors.right: parent.right
                anchors.rightMargin: 20
                opacity: 0
                Material.background: Material.Orange
                Material.foreground: "white"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: -50
                icon.source: "qrc:/assets/icons/icon_download.svg"
                icon.width: 12
                icon.height: 12
            }
        }

        OpacityMask {
            anchors.fill: itemWrapper
            antialiasing: true
            source: itemWrapper
            maskSource: mask

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {

                }
                onExited: {

                }
                onContainsMouseChanged: {
                    if (containsMouse) {
                        workshopItem.state = "hover"
                    } else {
                        workshopItem.state = ""
                    }
                }

                onClicked: {
                    button.icon.color = "orange"
                    button.display = AbstractButton.IconOnly
                    steamWorkshop.subscribeItem(workshopItem.steamID)
                }
            }
        }
    }

    states: [
        State {
            name: "hover"

            PropertyChanges {
                target: button
                opacity: 1
                anchors.bottomMargin: 10
            }

            PropertyChanges {
                target: txtTitle
                opacity: 1
                anchors.bottomMargin: 20
            }

            PropertyChanges {
                target: shadow
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "hover"
            reversible: true

            PropertyAnimation {
                target: button
                duration: 200
                properties: "opacity, anchors.bottomMargin"
            }
            PropertyAnimation {
                target: txtTitle
                duration: 200
                properties: "opacity, anchors.bottomMargin"
            }
            PropertyAnimation {
                target: shadow
                duration: 200
                properties: "opacity"
            }
        }
    ]
}
