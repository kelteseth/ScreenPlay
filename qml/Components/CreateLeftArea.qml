import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import Qt.labs.platform 1.0

Rectangle {
    id: leftArea
    radius: 3
    height: column.childrenRect.height + (6 * 30)
    state: "workshop"
    property int steamWorkshopHeight
    property url videoPath
    property url previewPath
    //Todo 5.10 ENUM
    signal hasEmptyField(int fieldNumber)

    Row {
        id: rowUseSteamWorkshop
        height: 40
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            margins: 30
        }

        Switch {
            id: switchUseSteamWorkshop
            checked: true
            onCheckedChanged: {
                if (switchUseSteamWorkshop.checked) {
                    leftArea.state = "workshop"
                } else {
                    leftArea.state = "local"
                }
            }
        }
        Text {
            id: txtUseSteamWorkshop
            text: qsTr("Upload Wallpaper to Steam Workshop")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 12
            renderType: Text.NativeRendering
            font.family: font_Roboto_Regular.name
        }
    }

    Column {
        id: column
        anchors {
            top: rowUseSteamWorkshop.bottom
            right: parent.right
            left: parent.left
            margins: 30
        }
        spacing: 30

        TextField {
            id: txtTitle
            width: parent.width
            height: 60
            text: qsTr("")
            placeholderText: "Title"
        }

        Column {
            spacing: 30
            id: useSteamWorkshopFieldsWrapper
            height: 300
            anchors {
                right: parent.right
                left: parent.left
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
        }

        Button {
            id: btnSubmit
            text: qsTr("Create New Workshop Wallpaper")
            onClicked: {

                //Check for empty fields
                if (videoPath.toString() === "") {
                    hasEmptyField(0)
                } else if (previewPath.toString() === "") {
                    hasEmptyField(1)
                } else {

                    //Check if whether to use steamWorkshop or not
                    if (switchUseSteamWorkshop.checked) {
                        //TODO wait for callback
                        steamWorkshop.createWorkshopItem()

                        steamWorkshop.submitWorkshopItem(
                                    txtTitle.text.toString(),
                                    txtDescription.text.toString(), "english",
                                    cbVisibility.currentIndex,
                                    fileDialogOpenVideo.currentFile,
                                    fileDialogOpenPreview.currentFile)
                        tiItemUpdate.start()
                    } else {

                        steamWorkshop.createLocalWorkshopItem(
                                    txtTitle.text.toString(),
                                    fileDialogOpenVideo.currentFile,
                                    fileDialogOpenPreview.currentFile)
                    }
                }
            }
        }
        CheckDelegate {
            id: checkDelegate
            opacity: 0
            visible: false
            text: qsTr("By submitting this item, you agree to the workshop terms of service")
            onCheckedChanged: {
                if (checkDelegate.visible) {
                    Qt.openUrlExternally(
                                "http://steamcommunity.com/sharedfiles/workshoplegalagreement")
                }
            }
        }
    }

    states: [
        State {
            name: "local"
            PropertyChanges {
                target: useSteamWorkshopFieldsWrapper
                visible: false
                height: 0
            }
            PropertyChanges {
                target: txtUseSteamWorkshop
                color: "black"
            }
            PropertyChanges {
                target: btnSubmit
                text: qsTr("Create Local Wallpaper")
            }
        },
        State {
            name: "workshop"
            PropertyChanges {
                target: useSteamWorkshopFieldsWrapper
                visible: true
                height: 200
            }
            PropertyChanges {
                target: txtUseSteamWorkshop
                color: "orange"
            }
            PropertyChanges {
                target: btnSubmit
                text: qsTr("Upload Wallpaper to Steam Workshop")
            }
        }
    ]
    transitions: [
        Transition {
            from: "local"
            to: "workshop"

            PropertyAnimation {
                property: "height"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "workshop"
            to: "local"

            PropertyAnimation {
                property: "height"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
