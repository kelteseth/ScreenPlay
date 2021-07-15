import QtQuick 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.labs.platform 1.1
import ScreenPlay 1.0
import "../../Common" as Common

WizardPage {
    id: root

    property url file

    sourceComponent: ColumnLayout {
        property bool ready: tfTitle.text.length >= 1 && root.file.length !== ""

        function create() {
            ScreenPlay.wizards.createGifWallpaper(tfTitle.text, cbLicense.name, cbLicense.licenseFile, tfCreatedBy.text, root.file, tagSelector.getTags());
        }

        onReadyChanged: root.ready = ready

        Common.Headline {
            id: txtHeadline

            text: qsTr("Import a Gif Wallpaper")
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
                Layout.preferredHeight: root.width * 0.5
                Layout.preferredWidth: root.width * 0.5

                Rectangle {
                    id: leftWrapper

                    color: Qt.darker(Material.backgroundColor)
                    radius: 3
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    DropArea {
                        id: dropArea

                        anchors.fill: parent
                        onDropped: {
                            root.file = drop.urls[0];
                            leftWrapper.color = Qt.darker(Qt.darker(Material.backgroundColor));
                        }
                        onExited: {
                            leftWrapper.color = Qt.darker(Material.backgroundColor);
                        }
                        onEntered: {
                            leftWrapper.color = Qt.darker(Qt.darker(Material.backgroundColor));
                            drag.accept(Qt.LinkAction);
                        }
                    }

                    Image {
                        id: bgPattern

                        anchors.fill: parent
                        fillMode: Image.Tile
                        opacity: 0.2
                        source: "qrc:/assets/images/noisy-texture-3.png"
                    }

                    Text {
                        color: Material.primaryTextColor
                        font.family: ScreenPlay.settings.font
                        font.pointSize: 13
                        text: qsTr("Drop a *.gif file here or use 'Select file' below.")
                        anchors.centerIn: parent
                    }

                    AnimatedImage {
                        id: imgPreview

                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop
                        source: root.file
                    }

                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                }

                Common.FileSelector {
                    id: fileSelector

                    Layout.fillWidth: true
                    placeHolderText: qsTr("Select your gif")
                    fileDialog.nameFilters: ["Gif (*.gif)"]
                    onFileChanged: root.file = file
                }

            }

            ColumnLayout {
                id: rightWrapper

                spacing: 20
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * 0.5

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
                    id: tfCreatedBy

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
