import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id: wrapperContent

    property bool conversionFinishedSuccessful: false
    property bool canSave: false
    property var codec: Create.VP8
    property string filePath



    onFilePathChanged: {
        textFieldName.text =  basename(filePath)
    }

    function cleanup() {
        ScreenPlay.create.abortAndCleanup()
    }

    function basename(str)
    {
        let filenameWithExtentions = (str.slice(str.lastIndexOf("/")+1))
        let filename = filenameWithExtentions.split('.').slice(0, -1).join('.')
        return filename
    }


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
        target: ScreenPlay.create

        onCreateWallpaperStateChanged: {

            switch (state) {
            case CreateImportVideo.ConvertingPreviewImage:
                txtConvert.text = qsTr("Generating preview image...")
                break
            case CreateImportVideo.ConvertingPreviewThumbnailImage:
                txtConvert.text = qsTr("Generating preview thumbnail image...")
                break
            case CreateImportVideo.ConvertingPreviewImageFinished:
                imgPreview.source = "file:///" + ScreenPlay.create.workingDir + "/preview.jpg"
                imgPreview.visible = true
                break
            case CreateImportVideo.ConvertingPreviewVideo:
                txtConvert.text = qsTr("Generating 5 second preview video...")
                break
            case CreateImportVideo.ConvertingPreviewGif:
                txtConvert.text = qsTr("Generating preview gif...")
                break
            case CreateImportVideo.ConvertingPreviewGifFinished:
                gifPreview.source = "file:///" + ScreenPlay.create.workingDir + "/preview.gif"
                imgPreview.visible = false
                gifPreview.visible = true
                gifPreview.playing = true
                break
            case CreateImportVideo.ConvertingAudio:
                txtConvert.text = qsTr("Converting Audio...")
                break
            case CreateImportVideo.ConvertingVideo:
                txtConvert.text = qsTr(
                            "Converting Video... This can take some time!")
                break
            case CreateImportVideo.ConvertingVideoError:
                txtConvert.text = qsTr("Converting Video ERROR!")
                break
            case CreateImportVideo.AnalyseVideoError:
                txtConvert.text = qsTr("Analyse Video ERROR!")
                break
            case CreateImportVideo.Finished:
                txtConvert.text = ""
                conversionFinishedSuccessful = true
                busyIndicator.running = false
                wrapperContent.checkCanSave()

                ScreenPlay.setTrackerSendEvent("createWallpaperSuccessful", "");
                break
            }
        }
        onProgressChanged: {
            var percentage = Math.floor(progress * 100)

            if (percentage > 100 || progress > 0.95)
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
            anchors {
                top:parent.top
                right: parent.right
                rightMargin: 20
                bottom: previewSelector.top
                bottomMargin: 20
                left: parent.left
            }

            color: Material.color(Material.Grey)

            Image {
                fillMode: Image.PreserveAspectCrop
                id: imgPreview
                asynchronous: true
                visible: false
                anchors.fill: parent
            }

            AnimatedImage {
                id: gifPreview
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                playing: true
                visible: false
                anchors.fill: parent
            }

            LinearGradient {
                id: shadow
                cached: true
                anchors.fill: parent
                start: Qt.point(0, height)
                end: Qt.point(0, 0)
                gradient: Gradient {
                    GradientStop {
                        id: gradientStop0
                        position: 0.0
                        color: "#DD000000"
                    }
                    GradientStop {
                        id: gradientStop1
                        position: 1.0
                        color: "#00000000"
                    }
                }
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
            height: 80
            placeHolderText: qsTr("You can set your own preview image here!")
            anchors {
                right: parent.right
                rightMargin: 20
                left: parent.left
                bottom: parent.bottom
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
                right:parent.right
                rightMargin: 30
                bottomMargin: -10
                bottom: parent.bottom
            }

            Button {
                id: btnExit
                text: qsTr("Abort")
                Material.background: Material.Red
                Material.foreground: "white"
                onClicked: {
                    ScreenPlay.create.abortAndCleanup()
                    ScreenPlay.util.setNavigationActive(true)
                    ScreenPlay.util.setNavigation("Create")
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
                        btnSave.enabled = false
                        ScreenPlay.create.saveWallpaper(
                                    textFieldName.text,
                                    textFieldDescription.text,
                                    wrapperContent.filePath,
                                    previewSelector.imageSource,
                                    textFieldYoutubeURL.text,
                                    codec,
                                    textFieldTags.getTags())
                        savePopup.open()
                        ScreenPlay.installedListModel.reset()
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
            interval: 1000 + Math.random() * 1000
            onTriggered: {
                savePopup.close()
                ScreenPlay.util.setNavigationActive(true)
                ScreenPlay.util.setNavigation("Installed")
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:580;width:1200}
}
##^##*/

