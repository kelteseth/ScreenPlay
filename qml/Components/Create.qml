import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

CustomPage {
    id: page
    pageName: ""

    Connections {
        target: steamWorkshop
        onWorkshopItemCreated: {
            print(userNeedsToAcceptWorkshopLegalAgreement + " " + eResult + " " + publishedFileId)
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
                id: text1
                text: qsTr("Title")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
                height: parent.height
            }

            TextField {
                id: textField
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
                id: text2
                height: parent.height
                text: qsTr("Description")
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }

            TextArea {
                id: textArea
                height: 120
                text: qsTr("")
                wrapMode: Text.WrapAnywhere
                width: parent.width - text2.width - 30

            }
            spacing: 30
        }
        Row {
            id: row2
            width: parent.width
            height: 120
            Text {
                id: text3
                height: parent.height
                text: qsTr("Description")
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }

            CheckDelegate {
                id: checkDelegateVisible
                text: qsTr("Is item visible")
            }
            spacing: 30
        }

        Row {
            id: row4
            width: parent.width
            height: 120
            Text {
                id: text4
                height: parent.height
                text: qsTr("Video File")
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }

            Button {
                id: btnOpenVideo
                text: qsTr("Open Video ")
                onClicked: {
                   //openFileDialogLoader.source ="qrc:/qml/Components/OpenFileDialog.qml"
                    fileDialog.open()
                }
            }

            FileDialog {
                id: fileDialog
                nameFilters: ["Videos (*.mp4)"]
                onAccepted: {
                    console.log("You chose: " + fileDialog.fileUrls)
                }
            }



            spacing: 30
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
