import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import ScreenPlay.Workshop 1.0 as SP

import ScreenPlay 1.0

Item {
    id: root
    width: 320
    height: 180

    property url imgUrl
    property url additionalPreviewUrl
    property string name
    property int workshopID
    property int itemIndex
    property int subscriptionCount

    property bool isDownloading: false

    signal clicked(int workshopID, url imgUrl)

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
    Timer {
        id: timerAnim
        interval: 40 * itemIndex * Math.random()
        running: true
        repeat: false
        onTriggered: showAnim.start()
    }

    transform: [
        Rotation {
            id: rt
            origin.x: width * .5
            origin.y: height * .5
            axis {
                x: -.5
                y: 0
                z: 0
            }
            angle: 0
        },
        Translate {
            id: tr
        },
        Scale {
            id: sc
            origin.x: width * .5
            origin.y: height * .5
        }
    ]
    ParallelAnimation {
        id: showAnim
        running: false
        RotationAnimation {
            target: rt
            from: 90
            to: 0
            duration: 500
            easing.type: Easing.OutQuint
            property: "angle"
        }
        PropertyAnimation {
            target: root
            from: 0
            to: 1
            duration: 500
            easing.type: Easing.OutQuint
            property: "opacity"
        }
        PropertyAnimation {
            target: tr
            from: 80
            to: 0
            duration: 500
            easing.type: Easing.OutQuint
            property: "y"
        }
        PropertyAnimation {
            target: sc
            from: .8
            to: 1
            duration: 500
            easing.type: Easing.OutQuint
            properties: "xScale,yScale"
        }
    }

    Item {
        id: screenPlay
        anchors.centerIn: parent
        height: 180
        width: 320

        Image {
            id: mask
            source: "qrc:/assets/images/Window.svg"
            sourceSize: Qt.size(screenPlay.width, screenPlay.height)
            visible: false
            smooth: true
            fillMode: Image.PreserveAspectFit
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
                sourceImage: root.imgUrl
                sourceImageGIF: root.additionalPreviewUrl
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
                text: root.name

                opacity: 0
                height: 30
                width: 180
                verticalAlignment: Text.AlignVCenter
                color: "white"
                font.pointSize: 18
                wrapMode: Text.WordWrap
                font.family: ScreenPlay.settings.font
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
                anchors {
                    right: parent.right
                    rightMargin: 20
                    bottom: parent.bottom
                    bottomMargin: -50
                }
                opacity: 0
                Material.background: Material.Orange
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_download.svg"
                icon.width: 12
                icon.height: 12
            }

            Item {
                id: openInWorkshop
                height: 20
                width: 20
                z: 99
                opacity: 0
                anchors {
                    margins: 10
                    top: parent.top
                    right: parent.right
                }
                Image {
                    source: "qrc:/assets/icons/icon_open_in_new.svg"
                    sourceSize: Qt.size(parent.width, parent.height)
                    fillMode: Image.PreserveAspectFit
                }
            }
        }

        OpacityMask {
            anchors.fill: itemWrapper
            source: itemWrapper
            maskSource: mask

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onContainsMouseChanged: {
                    if (!isDownloading) {
                        if (containsMouse) {
                            root.state = "hover"
                        } else {
                            root.state = ""
                        }
                    }
                }
                onClicked: {
                    root.clicked(root.workshopID, root.imgUrl)
                }
            }
            MouseArea {
                cursorShape: Qt.PointingHandCursor
                height: 50
                anchors {
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }

                onClicked: {
                    isDownloading = true
                    root.state = "downloading"
                    SP.Workshop.steamWorkshop.subscribeItem(root.workshopID)
                    ScreenPlay.setTrackerSendEvent("subscribeItem",
                                                   root.workshopID)
                }

                Connections {
                    target: SP.Workshop.steamWorkshop
                    function onWorkshopItemInstalled(appID, publishedFile) {
                        if (appID === SP.Workshop.steamWorkshop.appID) {
                            root.state = "installed"
                        }
                    }
                }
            }

            MouseArea {
                height: 20
                width: 20
                cursorShape: Qt.PointingHandCursor
                anchors {
                    margins: 10
                    top: parent.top
                    right: parent.right
                }
                onClicked: {
                    Qt.openUrlExternally(
                                "steam://url/CommunityFilePage/" + root.workshopID)
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
                text: qsTr("Successfully subscribed to Workshop Item!")
                color: "white"
                font.pointSize: 18
                wrapMode: Text.WordWrap
                font.family: ScreenPlay.settings.font
                horizontalAlignment: Qt.AlignHCenter
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: 20
                    left: parent.left
                    leftMargin: 20
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
                target: openInWorkshop
                opacity: .75
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
                target: openInWorkshop
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
        },
        State {
            name: "installed"

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
            PropertyChanges {
                target: txtDownloading
                text: qsTr("Download complete!")
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
                target: openInWorkshop
                duration: 100
                properties: "opacity"
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
                    duration: 500
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
