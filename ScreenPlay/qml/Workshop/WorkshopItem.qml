import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import ScreenPlay.Workshop 1.0
import ScreenPlay 1.0

Item {
    id: root

    property url imgUrl
    property url additionalPreviewUrl
    property string name
    property var publishedFileID: 0
    property int itemIndex
    property int subscriptionCount
    property bool isDownloading: false
    property SteamWorkshop steamWorkshop

    signal clicked(var publishedFileID, url imgUrl)

    width: 320
    height: 180
    transform: [
        Rotation {
            id: rt

            origin.x: width * 0.5
            origin.y: height * 0.5
            angle: 0

            axis {
                x: -0.5
                y: 0
                z: 0
            }

        },
        Translate {
            id: tr
        },
        Scale {
            id: sc

            origin.x: width * 0.5
            origin.y: height * 0.5
        }
    ]

    RectangularGlow {
        id: effect

        height: parent.height
        width: parent.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15

        anchors {
            top: parent.top
            topMargin: 3
        }

    }

    Timer {
        id: timerAnim

        interval: 40 * itemIndex * Math.random()
        running: true
        repeat: false
        onTriggered: showAnim.start()
    }

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
            from: 0.8
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
                start: Qt.point(0, 80)
                end: Qt.point(0, 0)

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                }

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#CC000000"
                    }

                    GradientStop {
                        position: 1
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
                font.pointSize: 14
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.family: ScreenPlay.settings.font

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 20
                    left: parent.left
                    leftMargin: 20
                    bottomMargin: -50
                }

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
                        if (containsMouse)
                            root.state = "hover";
                        else
                            root.state = "";
                    }
                }
                onClicked: {
                    root.clicked(root.publishedFileID, root.imgUrl);
                }
            }

            MouseArea {
                height: 20
                width: 20
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    Qt.openUrlExternally("steam://url/CommunityFilePage/" + root.publishedFileID);
                }

                anchors {
                    margins: 10
                    top: parent.top
                    right: parent.right
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
                target: openInWorkshop
                opacity: 0.75
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
