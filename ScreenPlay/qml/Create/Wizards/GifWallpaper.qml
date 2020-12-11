import QtQuick 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.2
import ScreenPlay 1.0

import "../../Common" as Common

WizardPage {
    id: root
    sourceComponent: ColumnLayout {

        function create() {
            ScreenPlay.wizards.createGifWallpaper(tfTitle.text,
                                                  cbLicense.currentText,
                                                  tfCreatedBy.text,
                                                  tagSelector.getTags())
        }

        property bool ready: tfTitle.text.length >= 1
                             && fileSelector.file.length > 1
        onReadyChanged: root.ready = ready

        Common.Headline {
            id: txtHeadline
            text: qsTr("Create an Gif Wallpaper")
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 20
        }

        RowLayout {
            spacing: 20
            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout {
                Layout.preferredHeight: root.width * .5
                Layout.preferredWidth: root.width * .5

                Rectangle {
                    id: leftWrapper
                    color: "#333333"
                    radius: 3
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    AnimatedImage {
                        id: imgPreview
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                }

                Common.FileSelector {
                    id: fileSelector
                    placeHolderText: qsTr("Select your gif")
                    fileDialog.nameFilters: ["Gif (*.gif)"]
                    fileDialog.onAccepted: {
                        imgPreview.source = fileSelector.fileDialog.file
                    }

                    Layout.fillWidth: true
                }
            }

            ColumnLayout {
                id: rightWrapper
                spacing: 20
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * .5

                Common.HeadlineSection {
                    text: qsTr("General")
                }

                Common.TextField {
                    id: tfTitle
                    Layout.fillWidth: true
                    placeholderText: qsTr("Wallpaper name")
                    required: true
                }

                Common.TextField {
                    id: tfLicense
                    Layout.fillWidth: true
                    placeholderText: qsTr("Created By")
                }

                Common.LicenseSelector {
                    id: cbLicense
                    Layout.fillWidth: true
                }

                Common.HeadlineSection {
                    text: qsTr("Tags")
                }

                Common.TagSelector {
                    id: tagSelector
                    Layout.fillWidth: true
                }
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:580;width:1200}
}
##^##*/

