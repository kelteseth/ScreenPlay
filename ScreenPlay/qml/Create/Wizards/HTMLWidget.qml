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
            ScreenPlay.wizards.createHTMLWidget(tfTitle.text, cbLicense.name, cbLicense.licenseFile, tfCreatedBy.text, previewSelector.imageSource, tagSelector.getTags());
        }

        Common.Headline {
            id: txtHeadline

            text: qsTr("Create a HTML widget")
            Layout.fillWidth: true
        }

        Item {
            Layout.preferredHeight: 20
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 20
            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout {
                Layout.preferredHeight: root.width * 0.5
                Layout.preferredWidth: root.width * 0.5
                spacing: 20

                Rectangle {
                    id: leftWrapper

                    color: "#333333"
                    radius: 3
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Image {
                        id: imgPreview

                        source: "qrc:/assets/wizards/example_html.png"
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop
                    }

                }

                Common.ImageSelector {
                    id: previewSelector

                    Layout.fillWidth: true
                }

            }

            ColumnLayout {
                id: rightWrapper

                spacing: 20
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * 0.5
                Layout.alignment: Qt.AlignTop

                Common.HeadlineSection {
                    text: qsTr("General")
                }

                Common.TextField {
                    id: tfTitle

                    Layout.fillWidth: true
                    required: true
                    placeholderText: qsTr("Widget name")
                    onTextChanged: root.ready = text.length >= 1
                }

                Common.TextField {
                    id: tfCreatedBy

                    Layout.fillWidth: true
                    placeholderText: qsTr("Created by")
                }

                Common.LicenseSelector {
                    id: cbLicense
                }

                Common.HeadlineSection {
                    text: qsTr("Tags")
                }

                Common.TagSelector {
                    id: tagSelector

                    Layout.fillWidth: true
                }

            }

        }

    }

}
