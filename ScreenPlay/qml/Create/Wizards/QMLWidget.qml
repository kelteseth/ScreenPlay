import QtQuick
import QtQuick.Controls.Material
import QtQuick.Controls
import QtQuick.Layouts

import ScreenPlayApp
import ScreenPlay
import ScreenPlayUtil as Util

WizardPage {
    id: root

    sourceComponent: ColumnLayout {
        function create() {
            App.wizards.createQMLWidget(tfTitle.text, cbLicense.name, cbLicense.licenseFile, tfCreatedBy.text, previewSelector.imageSource, tagSelector.getTags());
        }

        Util.Headline {
            id: txtHeadline

            text: qsTr("Create a QML widget")
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

                        source: "qrc:/qml/ScreenPlayApp/assets/wizards/example_qml.png"
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop
                    }

                }

                Util.ImageSelector {
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

                Util.HeadlineSection {
                    text: qsTr("General")
                }

                Util.TextField {
                    id: tfTitle

                    Layout.fillWidth: true
                    required: true
                    placeholderText: qsTr("Widget name")
                    onTextChanged: root.ready = text.length >= 1
                }

                Util.TextField {
                    id: tfCreatedBy

                    Layout.fillWidth: true
                    placeholderText: qsTr("Created by")
                }

                Util.LicenseSelector {
                    id: cbLicense
                }

                Util.HeadlineSection {
                    text: qsTr("Tags")
                }

                Util.TagSelector {
                    id: tagSelector

                    Layout.fillWidth: true
                }

            }

        }

    }

}
