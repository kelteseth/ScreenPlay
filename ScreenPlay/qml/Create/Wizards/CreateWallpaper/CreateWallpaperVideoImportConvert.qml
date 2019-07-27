import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12

import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id: wrapperContent

    property bool conversionFinishedSuccessful: false
    property bool canSave: false
    property string filePath
    onCanSaveChanged: wrapperContent.checkCanSave()
    signal save

    function checkCanSave() {
        if (canSave && conversionFinishedSuccessful) {
            btnSave.enabled = true
        } else {
            btnSave.enabled = false
        }
    }

    Connections {
        target: screenPlayCreate

        onCreateWallpaperStateChanged: {

            switch (state) {
            case CreateImportVideo.ConvertingPreviewImageFinished:
                imgPreview.source = "file:///" + screenPlayCreate.workingDir + "/preview.jpg"
                imgPreview.visible = true
                txtConvert.text = qsTr("Converting Video preview mp4")
                break
            case CreateImportVideo.ConvertingPreviewVideo:
                txtConvert.text = qsTr("Generating preview video...")
                break
            case CreateImportVideo.ConvertingPreviewGif:
                txtConvert.text = qsTr("Generating preview gif...")
                break
            case CreateImportVideo.ConvertingPreviewGifFinished:
                gifPreview.source = "file:///" + screenPlayCreate.workingDir + "/preview.gif"
                imgPreview.visible = false
                gifPreview.visible = true
                gifPreview.playing = true
                break
            case CreateImportVideo.ConvertingAudio:
                txtConvert.text = qsTr("Converting Audio...")
                break
            case CreateImportVideo.ConvertingVideo:
                txtConvert.text = qsTr("Converting Video... This can take some time!")
                break
            case CreateImportVideo.ConvertingVideoError:
                txtConvert.text = qsTr("Converting Video ERROR!")
                break
            case CreateImportVideo.Finished:
                txtConvert.text = ""
                conversionFinishedSuccessful = true
                busyIndicator.running = false
                wrapperContent.checkCanSave()
                break
            }
        }
        onProgressChanged: {
            var percentage = Math.floor(progress * 100)
            if (percentage > 100)
                percentage = 100
            txtConvertNumber.text = percentage + "%"
        }
    }

    Text {
        id: txtHeadline
        text: qsTr("Convert a video to a wallpaper")
        height: 40
        font.family: "Roboto"
        font.weight: Font.Light
        color: "#757575"

        font.pointSize: 23
        anchors {
            top: parent.top
            left: parent.left
            margins: 40
            bottomMargin: 0
        }
    }

    Item {
        id: wrapperLeft
        width: parent.width * .5
        anchors {
            left: parent.left
            top: txtHeadline.bottom
            margins: 30
            bottom: parent.bottom
        }

        Rectangle {
            id: imgWrapper
            width: 425
            height: 247
            color: Material.color(Material.Grey)
            anchors {
                top: parent.top
                left: parent.left
            }

            Image {
                id: imgPreview
                asynchronous: true
                visible: false
                anchors.fill: parent
            }

            AnimatedImage {
                id: gifPreview
                asynchronous: true
                playing: true
                visible: false
                anchors.fill: parent
            }

            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent
                running: true
            }

            Text {
                id: txtConvertNumber
                color: "white"
                text: qsTr("")
                font.pointSize: 21
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 40
                }
            }

            Text {
                id: txtConvert
                color: "white"
                text: qsTr("Generating preview video...")
                font.pointSize: 14
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
            }
        }
        ImageSelector {
            id: previewSelector
            placeHolderText: qsTr("You can set your own preview image here!")
            anchors {
                top: imgWrapper.bottom
                topMargin: 20
                right: parent.right
                rightMargin: 30
                left: parent.left
            }
        }
    }
    Item {
        id: wrapperRight
        width: parent.width * .5
        anchors {
            top: txtHeadline.bottom
            topMargin: 30
            bottom: parent.bottom
            right: parent.right
        }

        ColumnLayout {
            id: column
            spacing: 0
            anchors {

                right: parent.right
                left: parent.left
                margins: 30
                top: parent.top
                topMargin: 0
                bottom: column1.top
                bottomMargin: 50
            }

            TextField {
                id: textFieldName
                placeholderText: qsTr("Name (required!)")
                width: parent.width
                Layout.fillWidth: true
                onTextChanged: {
                    if (textFieldName.text.length >= 3) {
                        canSave = true
                    } else {
                        canSave = false
                    }
                }
            }

            TextField {
                id: textFieldDescription
                placeholderText: qsTr("Description")
                width: parent.width
                Layout.fillWidth: true
            }

            TextField {
                id: textFieldYoutubeURL
                placeholderText: qsTr("Youtube URL")
                width: parent.width
                Layout.fillWidth: true
            }

            TagSelector {
                id: textFieldTags
                width: parent.width
                Layout.fillWidth: true
            }
        }

        Row {
            id: column1
            height: 80
            width: childrenRect.width
            spacing: 10
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            Button {
                id: btnExit
                text: qsTr("Abort")
                Material.background: Material.Red
                Material.foreground: "white"
                onClicked: {
                    screenPlayCreate.abortAndCleanup()
                    utility.setNavigationActive(true)
                    utility.setNavigation("Create")
                }
            }

            Button {
                id: btnSave
                text: qsTr("Save")
                enabled: false
                Material.background: Material.Orange
                Material.foreground: "white"

                onClicked: {
                    if (conversionFinishedSuccessful) {
                        screenPlayCreate.saveWallpaper(
                                    textFieldName.text,
                                    textFieldDescription.text,
                                    wrapperContent.filePath,
                                    previewSelector.imageSource,
                                    textFieldYoutubeURL.text,
                                    textFieldTags.getTags())
                        savePopup.open()
                    }
                }
            }
        }
    }
    Popup {
        id: savePopup
        modal: true
        focus: true
        width: 250
        anchors.centerIn: parent
        height: 200
        onOpened: timerSave.start()

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }
        Text {
            text: qsTr("Save Wallpaper...")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
        }

        Timer {
            id: timerSave
            interval: 3000 - Math.random() * 1000
            onTriggered: {
                utility.setNavigationActive(true)
                utility.setNavigation("Create")
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:950}
}
 ##^##*/

