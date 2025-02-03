import QtQuick
import QtQuick.Controls.Material
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlay

import ScreenPlayCore as Util

WizardPage {
    id: root

    property url file

    sourceComponent: ColumnLayout {
        property bool ready: tfTitle.text.length >= 1 && root.file.length !== ""

        function create() {
            App.wizards.createGifWallpaper(tfTitle.text, cbLicense.name, cbLicense.licenseFile, tfCreatedBy.text, root.file, tagSelector.getTags()).then(result => {
                wizardFinished(result.success, result.message);
            });
        }

        onReadyChanged: root.ready = ready

        Util.Headline {
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
                        source: "qrc:/qt/qml/ScreenPlay/assets/images/noisy-texture-3.png"
                    }

                    Text {
                        color: Material.primaryTextColor
                        font.family: App.settings.font
                        font.pointSize: 13
                        text: qsTr("Drop a *.gif file here or use 'Select file' below.")
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width
                        anchors {
                            fill: parent
                            margins: 40
                        }
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

                Util.FileSelector {
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

                Util.HeadlineSection {
                    text: qsTr("General")
                }

                Util.TextField {
                    id: tfTitle

                    Layout.fillWidth: true
                    placeholderText: qsTr("Wallpaper name")
                    required: true
                }

                Util.TextField {
                    id: tfCreatedBy

                    Layout.fillWidth: true
                    placeholderText: qsTr("Created By")
                }

                Util.LicenseSelector {
                    id: cbLicense

                    Layout.fillWidth: true
                }

                Util.HeadlineSection {
                    text: qsTr("Tags")
                }

                Util.TagSelector {
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
