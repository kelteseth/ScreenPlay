import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import net.aimber.create 1.0

Rectangle {
    id: root

    property bool canNext: false
    property bool gifCreated: false

    Item {
        id: rectangle1
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        RectangularGlow {
            id: effect
            anchors {
                top: imgWrapper.top
                left: imgWrapper.left
                right: imgWrapper.right
                topMargin: 3
            }

            height: imgWrapper.height
            width: imgWrapper.width
            cached: true
            glowRadius: 3
            spread: 0.2
            color: "black"
            opacity: 0.4
            cornerRadius: 15
        }

        Rectangle {
            id: imgWrapper
            width: parent.width * .9
            anchors {
                top: parent.top
                margins: parent.width * .05
                right: parent.right
                left: parent.left
            }

            height: 200
            color: "gray"

            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent
                running: true
            }

            Text {
                id: text1
                color: "white"
                text: qsTr("Generating preview...")
                font.pixelSize: 14
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 30
                }
            }

            Connections {
                target: screenPlayCreate

                onCreateWallpaperStateChanged: {
                    if (state === Create.State.ConvertingPreviewGifFinished) {
                        imgPreview.source = "file:///"
                                + screenPlayCreate.workingDir + "/preview.gif"
                        imgPreview.visible = true
                        gifCreated = true
                    }
                }
            }

            AnimatedImage {
                id: imgPreview
                asynchronous: true
                playing: true
                visible: false
                anchors.fill: parent
            }
        }

        ScrollView {
            anchors {
                top: imgWrapper.bottom
                right: parent.right
                bottom: parent.bottom
                left: parent.left
                margins: 20
            }
            Text {
                id: txtOut
                width: parent.width
            }
        }
    }

    Rectangle {
        id: rectangle
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        Column {
            id: column
            spacing: 20
            anchors.fill: parent
            anchors.margins: 30

            TextField {
                id: textField
                placeholderText: qsTr("Name")
                anchors.right: parent.right
                anchors.left: parent.left
                onTextChanged: {
                    if (textField.text.length >= 3 && gifCreated) {
                        canNext = true
                    } else {
                        canNext = false
                    }
                }
            }

            TextField {
                id: textField1
                placeholderText: qsTr("Description")
                anchors.right: parent.right
                anchors.left: parent.left
            }

            TextField {
                id: textField2
                placeholderText: qsTr("Youtube URL")
                anchors.right: parent.right
                anchors.left: parent.left
            }

            TextField {
                id: textField3
                placeholderText: qsTr("Tags")
                anchors.right: parent.right
                anchors.left: parent.left
            }
        }
    }
}
