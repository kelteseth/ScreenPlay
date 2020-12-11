import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../Common" as Common

WizardPage {
    id: root

    sourceComponent: ColumnLayout {
        id: rightWrapper
        spacing: 10
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        function create() {
            ScreenPlay.wizards.createQMLWallpaper(
                        tfTitle.text, previewSelector.imageSource,
                        cbLicense.currentText, tagSelector.getTags())
        }

        Common.Headline {
            text: qsTr("Create a QML Wallpaper")
        }

        Common.HeadlineSection {
            text: qsTr("General")
        }
        RowLayout {
            spacing: 20

            Common.TextField {
                id: tfTitle
                Layout.fillWidth: true
                placeholderText: qsTr("Wallpaper name")
                required: true
                onTextChanged: root.ready = text.length >= 1
            }
            Common.TextField {
                id: tfCreatedBy
                Layout.fillWidth: true
                placeholderText: qsTr("Created By")
            }
        }
        Common.TextField {
            id: tfDescription
            Layout.fillWidth: true
            placeholderText: qsTr("Description")
        }

        Item {
            height: 30
        }

        Common.HeadlineSection {
            text: qsTr("License & Tags")
        }

        RowLayout {
            spacing: 20

            ComboBox {
                id: cbLicense
                Layout.fillWidth: true
                font.family: ScreenPlay.settings.font
                model: ListModel {
                    id: modelLicense
                    ListElement {
                        text: "Copyright by me"
                    }
                    ListElement {
                        text: "Open Source - GPLv3"
                    }
                    ListElement {
                        text: "Open Source - MIT/Apache2"
                    }
                }
            }

            Common.TagSelector {
                id: tagSelector
                Layout.fillWidth: true
            }
        }

        Item {
            height: 30
        }

        Common.HeadlineSection {
            text: qsTr("Preview Image")
        }

        Common.ImageSelector {
            id: previewSelector
            Layout.fillWidth: true
        }

    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

