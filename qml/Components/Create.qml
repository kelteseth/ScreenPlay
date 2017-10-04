import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0

CustomPage {
    id: page
    pageName: ""

    Connections {
        target: steamWorkshop
        onWorkshopItemCreated: {
            print("created")
            if (userNeedsToAcceptWorkshopLegalAgreement) {
                checkDelegate.opacity = 1
            } else {
                checkDelegate.opacity = 0
                busyIndicator.running = false
                page.state = "StartItemUpdate"
            }
        }
    }

    Item {
        id: left
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
                height: 120
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
        id: right
        width: parent.width * .5
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: 30
        }
        Row {
            id: rightTop
            height: 200
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
            spacing: 30
            Rectangle {
                color: "orange"
                width: parent.width * .4
                height: parent.height
            }
            Rectangle {
                color: "steelblue"
                width: parent.width * .4
                height: parent.height
            }
        }
        Rectangle {
            id: rightBottom
            height: 400
            color: "gray"
            anchors {
                top: rightTop.bottom
                right: parent.right
                left:parent.left
                bottom: parent.bottom
            }
        }
    }

    /*
        Row {
            id: row4
            width: parent.width
            height: 120
            Text {
                id: txtVideoFileDescription
                height: parent.height
                text: qsTr("Video File")
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }

            Button {
                id: btnOpenVideo
                text: qsTr("Open Video ")
                onClicked: {
                     fileDialogOpenVideo.open()
                }
            }

            FileDialog {
                id: fileDialogOpenVideo
                nameFilters: ["Videos (*.mp4)"]
                onAccepted: {

                }
            }



            spacing: 30
        }
        Row {
            width: parent.width
            height: 120
            Text {
                id: txtPreviewFileDescription
                height: parent.height
                text: qsTr("Video File preview")
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }

            Button {
                id: btnOpenPreview
                text: qsTr("Open Preview Image")
                onClicked: {
                     fileDialogOpenPreview.open()
                }
            }

            FileDialog {
                id: fileDialogOpenPreview
                nameFilters: ["*.png *.jpg *.gif","PNG (*.png)","JPG (*.jpg)","GIF (*.gif)"]
                onAccepted: {

                }
            }

            spacing: 30
        }*/
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
