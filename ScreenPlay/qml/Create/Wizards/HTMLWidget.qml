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
            ScreenPlay.wizards.createHTMLWidget(tfTitle.text,
                                                previewSelector.imageSource,
                                                tfCreatedBy.text,
                                                cbLicense.currentText,
                                                tagSelector.getTags())
        }

        Common.Headline {
            id: txtHeadline
            text: qsTr("Create an empty HTML widget")
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
                Layout.preferredHeight: root.width * .5
                Layout.preferredWidth: root.width * .5
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
                    id: imageSelector
                    Layout.fillWidth: true
                }
            }

            ColumnLayout {
                id: rightWrapper
                spacing: 20
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * .5
                Layout.alignment: Qt.AlignTop

                Common.HeadlineSection {
                    text: qsTr("General")
                }
                Common.TextField {
                    id: tfTitle
                    Layout.fillWidth: true
                    required: true
                    placeholderText: qsTr("Widget name")
                    onTextChanged: {
                        if (text.length >= 3) {
                            btnSave.enabled = true
                        } else {
                            btnSave.enabled = false
                        }
                    }
                }
                Common.TextField {
                    id: tfCreatedBy
                    Layout.fillWidth: true
                    placeholderText: qsTr("Created by")
                }

                Common.HeadlineSection {
                    text: qsTr("License")
                }
                ComboBox {
                    id: cbLicense
                    Layout.fillWidth: true
                    font.family: ScreenPlay.settings.font
                    model: ListModel {
                        id: modelLicense
                        ListElement {
                            text: "All rights "
                        }
                        ListElement {
                            text: "Open Source - GPLv3"
                        }
                        ListElement {
                            text: "Open Source - MIT/Apache2"
                        }
                    }
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

/*##^##
Designer {
    D{i:0;autoSize:true;height:580;width:1200}
}
##^##*/

