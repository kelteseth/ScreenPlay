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

    property bool isDownloading: false

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
                height: 80
                opacity: 0
                cached: true
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                }
                start: Qt.point(0, 80)
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
                opacity: 0
                height: 30
                width: 180
                verticalAlignment: Text.AlignVCenter
                color: "white"
                font.pixelSize: 18
                wrapMode: Text.WordWrap
                font.family: "Roboto"
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
                text: qsTr("Download")
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
                cursorShape: Qt.PointingHandCursor
                onContainsMouseChanged: {
                    if (!isDownloading) {
                        if (containsMouse) {
                            workshopItem.state = "hover"
                        } else {
                            workshopItem.state = ""
                        }
                    }
                }

                onClicked: {
                    isDownloading = true
                    workshopItem.state = "downloading"
                    steamWorkshop.subscribeItem(workshopItem.steamID)
                }
            }
        }
        FastBlur {
            id: effBlur
            anchors.fill: itemWrapper
            source: itemWrapper
            radius: 0
        }

        Item {
            id: itmDownloading
            opacity: 0
            anchors {
                top: parent.top
                topMargin: 50
                right: parent.right
                bottom: parent.bottom
                left: parent.left
            }

            Text {
                id: txtDownloading
                text: qsTr("Downloading Workshop content shortly!")
                color: "white"
                font.pixelSize: 21
                wrapMode: Text.WordWrap
                font.family: "Roboto"
                horizontalAlignment: Qt.AlignHCenter
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: 20
                    left: parent.left
                    leftMargin: 20
                }
            }

            ProgressBar {
                id: pbDownloading
                indeterminate: true
                anchors {
                    bottom: parent.bottom
                    bottomMargin: 20
                    horizontalCenter: parent.horizontalCenter
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
            PropertyChanges {
                target: effBlur
                radius: 0
            }
        },
        State {
            name: "downloading"

            PropertyChanges {
                target: button
                opacity: 0
            }

            PropertyChanges {
                target: txtTitle
                opacity: 0
            }

            PropertyChanges {
                target: shadow
                opacity: 0
            }

            PropertyChanges {
                target: effBlur
                radius: 64
            }

            PropertyChanges {
                target: itmDownloading
                opacity: 1
                anchors.topMargin: 0
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
                duration: 100
                properties: "opacity, anchors.bottomMargin"
            }
            PropertyAnimation {
                target: txtTitle
                duration: 100
                properties: "opacity, anchors.bottomMargin"
            }
            PropertyAnimation {
                target: shadow
                duration: 100
                properties: "opacity"
            }
        },
        Transition {
            from: "*"
            to: "downloading"
            reversible: true

            PropertyAnimation {
                target: button
                duration: 100
                properties: "opacity"
            }
            PropertyAnimation {
                target: txtTitle
                duration: 100
                properties: "opacity"
            }
            PropertyAnimation {
                target: shadow
                duration: 100
                properties: "opacity"
            }
            SequentialAnimation {
                PropertyAnimation {
                    target: effBlur
                    duration: 200
                    properties: "radius"
                }
                PropertyAnimation {
                    target: txtTitle
                    duration: 200
                    properties: "opacity, anchors.topMargin"
                }
            }
        }
    ]
}
