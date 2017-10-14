import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.0

CustomPage {
    id: page
    pageName: ""

    Connections {
        target: steamWorkshop
        onWorkshopItemCreated: {
            if (userNeedsToAcceptWorkshopLegalAgreement) {
                checkDelegate.opacity = 1
            } else {
                checkDelegate.opacity = 0
                checkDelegate.checkable = false
                checkDelegate.enabled = false
                busyIndicator.running = false
            }
        }
    }

    Connections {
        target: leftArea
        onHasEmptyField:{
            if(fieldNumber === 0){
                fileDropperVideo.state = "error"
            } else if (fieldNumber === 1) {
                fileDropperPreview.state = "error"
            }
        }
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    FontLoader {
        id: font_LibreBaskerville
        source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
    }

    CreateLeftArea {
        id: leftArea
        width: parent.width * .5
        anchors {
            top: parent.top
            left: parent.left
            margins: 10
        }

    }

    Item {
        id: rightArea
        width: parent.width * .4
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: 30
        }
        Item {
            id: rightTop
            height: 150
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
            Item {
                width: parent.width * .46
                height: parent.height
                FileDropperSingleFile {
                    id:fileDropperVideo
                    anchors.fill: parent
                    z: 99
                    descriptionTitle: "Drop your video here"
                    isVideo: true
                    imagePath: "qrc:/assets/icons/icon_tv.svg"
                    onExternalFilePathChanged: leftArea.videoPath = fileDropperVideo.externalFilePath
                }
            }
            Item {
                width: parent.width * .46
                anchors.right: parent.right
                height: parent.height

                FileDropperSingleFile {
                    id:fileDropperPreview
                    anchors.fill: parent
                    z: 99
                    descriptionTitle: "Set Preview Image"
                    imagePath: "qrc:/assets/icons/icon_single_image.svg"
                    onExternalFilePathChanged: leftArea.previewPath = fileDropperPreview.externalFilePath
                }
            }

            Item {
                id: rightBottom
                height: 300
                anchors {
                    top: rightTop.bottom
                    topMargin: 30
                    right: parent.right
                    left: parent.left
                }
                FileDropperSingleFile {
                    anchors.fill: parent
                    z: 99
                    descriptionTitle: "Add additional images"
                    imagePath: "qrc:/assets/icons/icon_multiple_images.svg"

                }
            }
        }

        Timer {
            id: tiItemUpdate
            interval: 100
            running: false
            repeat: true
            onTriggered: {
                print(steamWorkshop.getItemUpdateProcess())
            }
        }
    }
}
