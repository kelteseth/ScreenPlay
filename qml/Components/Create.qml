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

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    FontLoader {
        id: font_LibreBaskerville
        source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
    }

    Item {
        id: leftArea
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        Column {
            id: column
            anchors.fill: parent
            anchors.margins: 30
            spacing: 30

            TextField {
                id: txtTitle
                width: parent.width
                height: 60
                text: qsTr("")
                placeholderText: "Title"
            }
            TextField {
                id: txtDescription
                width: parent.width
                height: 60
                text: qsTr("")
                placeholderText: "Description"
            }
            TextField {
                id: txtTags
                width: parent.width
                height: 60
                text: qsTr("")
                placeholderText: "Tags"
            }
            TextField {
                id: txtYouTube
                width: parent.width
                height: 60
                text: qsTr("")
                placeholderText: "YouTube Preview"
            }

            Row {
                id: rowVisible
                width: parent.width
                height: 50
                Text {
                    id: txtVisibleDescription
                    height: parent.height
                    text: qsTr("Visible")
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                }

                ComboBox {
                    id: cbVisibility
                    model: ["Public", "Friends only", "Private"]
                }

                spacing: 30
            }

            Button {
                id: btnSubmit
                text: qsTr("Create New Wallpaper")
                onClicked: {
                    //TODO wait for callback
                    steamWorkshop.createWorkshopItem()

                    steamWorkshop.submitWorkshopItem(
                                txtTitle.text.toString(),
                                txtDescription.text.toString(), "english",
                                cbVisibility.currentIndex,
                                fileDialogOpenVideo.currentFile,
                                fileDialogOpenPreview.currentFile)
                    tiItemUpdate.start()
                }
            }
        }
        CheckDelegate {
            id: checkDelegate
            opacity: 0
            text: qsTr("By submitting this item, you agree to the workshop terms of service")
            onCheckedChanged: Qt.openUrlExternally(
                                  "http://steamcommunity.com/sharedfiles/workshoplegalagreement")
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
                    anchors.fill: parent
                    z:99
                    descriptionTitle: "Set Video"
                    isVideo: true
                    imagePath: "qrc:/assets/icons/icon_tv.svg"
                }
            }
            Item {
                width: parent.width * .46
                anchors.right: parent.right
                height: parent.height

                FileDropperSingleFile {
                    anchors.fill: parent
                    z:99
                    descriptionTitle: "Set Preview Image"
                    imagePath: "qrc:/assets/icons/icon_single_image.svg"
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
                    z:99
                    descriptionTitle: "Add additional images"
                    imagePath: "qrc:/assets/icons/icon_multiple_images.svg"
                }
            }
        }

        Timer {
            id: tiItemUpdate
            interval: 500
            running: false
            repeat: true
            onTriggered: {
                print(steamWorkshop.getItemUpdateProcess())
            }
        }
    }
}
