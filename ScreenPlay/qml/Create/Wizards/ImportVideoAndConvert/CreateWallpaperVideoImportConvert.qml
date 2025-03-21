import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay

import ScreenPlayCore as Util

Item {
    id: root

    property bool conversionFinishedSuccessful: false
    property bool canSave: false
    property var codec: Util.Video.VideoCodec.H264
    property string filePath

    signal abort
    signal save

    function cleanup() {
        App.create.cancel();
    }

    function basename(str) {
        let filenameWithExtentions = (str.slice(str.lastIndexOf("/") + 1));
        let filename = filenameWithExtentions.split('.').slice(0, -1).join('.');
        return filename;
    }

    function checkCanSave() {
        if (canSave && conversionFinishedSuccessful)
            btnSave.enabled = true;
        else
            btnSave.enabled = false;
    }

    onCanSaveChanged: root.checkCanSave()
    onFilePathChanged: {
        textFieldName.text = basename(filePath);
    }

    Connections {
        function onCreateWallpaperStateChanged(state) {
            switch (state) {
            case Import.State.ConvertingPreviewImage:
                txtConvert.text = qsTr("Generating preview image...");
                break;
            case Import.State.ConvertingPreviewThumbnailImage:
                txtConvert.text = qsTr("Generating preview thumbnail image...");
                break;
            case Import.State.ConvertingPreviewImageFinished:
                imgPreview.source = "file:///" + App.create.workingDir + "/preview.jpg";
                imgPreview.visible = true;
                break;
            case Import.State.ConvertingPreviewVideo:
                txtConvert.text = qsTr("Generating 5 second preview video...");
                break;
            case Import.State.ConvertingPreviewGif:
                txtConvert.text = qsTr("Generating preview gif...");
                break;
            case Import.State.ConvertingPreviewGifFinished:
                gifPreview.source = "file:///" + App.create.workingDir + "/preview.gif";
                imgPreview.visible = false;
                gifPreview.visible = true;
                gifPreview.playing = true;
                break;
            case Import.State.ConvertingAudio:
                txtConvert.text = qsTr("Converting Audio...");
                break;
            case Import.State.ConvertingVideo:
                txtConvert.text = qsTr("Converting Video... This can take some time!");
                break;
            case Import.State.ConvertingVideoError:
                txtConvert.text = qsTr("Converting Video ERROR!");
                break;
            case Import.State.AnalyseVideoError:
                txtConvert.text = qsTr("Analyse Video ERROR!");
                break;
            case Import.State.Finished:
                txtConvert.text = "";
                conversionFinishedSuccessful = true;
                busyIndicator.running = false;
                btnExit.enabled = false;
                root.checkCanSave();
                break;
            }
        }

        function onProgressChanged(progress) {
            var percentage = Math.floor(progress * 100);
            if (percentage > 100 || progress > 0.95)
                percentage = 100;
            if (percentage === NaN)
                print(progress, percentage);
            txtConvertNumber.text = percentage + "%";
        }

        target: App.create
    }

    Text {
        id: txtHeadline

        text: qsTr("Convert a video to a wallpaper")
        height: 40
        font.family: App.settings.font
        font.weight: Font.Light
        color: Material.primaryTextColor
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

        width: parent.width * 0.66

        anchors {
            left: parent.left
            top: txtHeadline.bottom
            margins: 30
            bottom: parent.bottom
        }

        Rectangle {
            id: imgWrapper

            color: Material.color(Material.Grey)

            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 20
                bottom: previewSelector.top
                bottomMargin: 20
                left: parent.left
            }

            Image {
                id: imgPreview

                fillMode: Image.PreserveAspectCrop
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

            Rectangle {
                id: shadow
                anchors.fill: parent

                gradient: Gradient {
                    GradientStop {
                        id: gradientStop0
                        position: 1
                        color: "#DD000000"
                    }

                    GradientStop {
                        id: gradientStop1
                        position: 0
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
                font.pointSize: 21
                font.family: App.settings.font

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 40
                }
            }

            Text {
                id: txtConvert

                color: Material.secondaryTextColor
                text: qsTr("Generating preview video...")
                font.pointSize: 14
                font.family: App.settings.font

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
            }
        }

        Util.ImageSelector {
            id: previewSelector

            height: 80

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

        width: parent.width * 0.33

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

            Util.TextField {
                id: textFieldName

                placeholderText: qsTr("Name (required!)")
                width: parent.width
                Layout.fillWidth: true
                onTextChanged: {
                    if (textFieldName.text.length >= 3)
                        canSave = true;
                    else
                        canSave = false;
                }
            }

            Util.TextField {
                id: textFieldDescription

                placeholderText: qsTr("Description")
                width: parent.width
                Layout.fillWidth: true
            }

            Util.TextField {
                id: textFieldYoutubeURL

                placeholderText: qsTr("Youtube URL")
                width: parent.width
                Layout.fillWidth: true
            }

            Util.TagSelector {
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
                right: parent.right
                rightMargin: 30
                bottomMargin: -10
                bottom: parent.bottom
            }

            Button {
                id: btnExit

                text: qsTr("Abort")
                Material.accent: Material.color(Material.Red)
                highlighted: true
                font.family: App.settings.font
                onClicked: {
                    root.abort();
                    App.create.cancel();
                }
            }

            Button {
                id: btnSave
                objectName: "btnSave"
                text: qsTr("Save")
                enabled: false
                Material.background: Material.accent
                Material.foreground: "white"
                font.family: App.settings.font
                onClicked: {
                    if (conversionFinishedSuccessful) {
                        btnSave.enabled = false;
                        App.create.saveWallpaper(textFieldName.text, textFieldDescription.text, root.filePath, previewSelector.imageSource, textFieldYoutubeURL.text, root.codec, textFieldTags.getTags());
                        savePopup.open();
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
            color: Material.primaryTextColor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            font.family: App.settings.font
        }

        Timer {
            id: timerSave

            interval: 1000 + Math.random() * 1000
            onTriggered: {
                savePopup.close();
                App.uiAppStateSignals.setNavigationActive(true);
                App.uiAppStateSignals.setNavigation("Installed");
            }
        }
    }
}
