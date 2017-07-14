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
            if (userNeedsToAcceptWorkshopLegalAgreement) {
                checkDelegate.opacity = 1
            } else {
                checkDelegate.opacity = 0
                busyIndicator.running = false
                page.state = "StartItemUpdate"
            }
        }
    }

    CheckDelegate {
        id: checkDelegate
        x: 35
        y: 398
        opacity: 0
        text: qsTr("By submitting this item, you agree to the workshop terms of service")
        onCheckedChanged: Qt.openUrlExternally( "http://steamcommunity.com/sharedfiles/workshoplegalagreement")
    }

    Button {
        id: btnCreateWallpaper
        text: qsTr("Create New Wallpaper")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            steamWorkshop.createWorkshopItem()
            busyIndicator.running = true
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.bottom: btnCreateWallpaper.bottom
        anchors.bottomMargin: 80
        anchors.horizontalCenter: parent.horizontalCenter
        running: false
        Material.accent: Material.Orange
    }

    Column {
        id: column
        anchors.fill: parent
        opacity: 0
        anchors.margins: 30

        Row {
            id: row
            width: parent.width
            height: 60
            spacing: 30

            Text {
                id: txtHeadlineTitle
                text: qsTr("Title")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
                height: parent.height
            }

            TextField {
                id: txtTitle
                width: 240
                text: qsTr("")
                height: parent.height
            }
        }

        Row {
            id: row1
            width: parent.width
            height: 120
            Text {
                id: txtDescriptionHeadline
                height: parent.height
                text: qsTr("Description")
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }

            TextArea {
                id: txtDescription
                height: 120
                text: qsTr("")
                wrapMode: Text.WrapAnywhere
                width: parent.width - txtDescriptionHeadline.width - 30

            }
            spacing: 30
        }
        Row {
            id: row2
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
                id:cbVisibility
                model: ["Public", "Friends only", "Private"]
            }

            spacing: 30
        }

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
        }

        Button {
            id: btnSubmit
            text: "Submit"
            onClicked: {
                steamWorkshop.submitWorkshopItem(txtTitle.text.toString(),
                                                 txtDescription.text.toString(),
                                                 "english",
                                                 cbVisibility.currentIndex,
                                                 fileDialogOpenVideo.currentFile,
                                                 fileDialogOpenPreview.currentFile);
                tiItemUpdate.start()
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

    states: [
        State {
            name: "StartItemUpdate"

            PropertyChanges {
                target: btnCreateWallpaper
                opacity: 0
            }

            PropertyChanges {
                target: busyIndicator
                opacity: 0
            }

            PropertyChanges {
                target: column
                opacity: 1
            }

       }
    ]
}
