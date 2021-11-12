import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import "../../Common" as Common

WizardPage {
    id: root

    sourceComponent: ColumnLayout {
        id: rightWrapper

        function create() {
            ScreenPlay.wizards.createQMLWallpaper(tfTitle.text, cbLicense.name, cbLicense.licenseFile, tfCreatedBy.text, previewSelector.imageSource, tagSelector.getTags());
        }

        spacing: 10

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        Common.Headline {
            text: qsTr("Create a QML Wallpaper")
            Layout.fillWidth: true
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

            Common.LicenseSelector {
                id: cbLicense
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
