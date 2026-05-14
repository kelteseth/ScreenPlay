import QtQuick
import QtQuick.Layouts
import ScreenPlay

import ScreenPlayCore as SPCore

WizardPage {
    id: root

    sourceComponent: ColumnLayout {
        id: rightWrapper

        function create() {
            App.wizards.createHTMLWallpaper(tfTitle.text, cbLicense.name, cbLicense.licenseFile, tfCreatedBy.text, previewSelector.imageSource, tagSelector.getTags()).then(result => {
                wizardFinished(result.success, result.message)
            })
        }

        spacing: 10

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        SPCore.Headline {
            text: qsTr("Create a HTML Wallpaper")
            Layout.fillWidth: true
        }

        SPCore.HeadlineSection {
            text: qsTr("General")
        }

        RowLayout {
            spacing: 20

            SPCore.TextField {
                id: tfTitle

                Layout.fillWidth: true
                placeholderText: qsTr("Wallpaper name")
                required: true
                onTextChanged: root.ready = text.length >= 1
            }

            SPCore.TextField {
                id: tfCreatedBy

                Layout.fillWidth: true
                placeholderText: qsTr("Created By")
            }
        }

        SPCore.TextField {
            id: tfDescription

            Layout.fillWidth: true
            placeholderText: qsTr("Description")
        }

        Item {
            height: 10
        }

        SPCore.HeadlineSection {
            text: qsTr("License & Tags")
        }

        RowLayout {
            spacing: 20

            SPCore.LicenseSelector {
                id: cbLicense
            }

            SPCore.TagSelector {
                id: tagSelector

                Layout.fillWidth: true
            }
        }

        Item {
            height: 30
        }

        SPCore.HeadlineSection {
            text: qsTr("Preview Image")
        }

        SPCore.ImageSelector {
            id: previewSelector

            Layout.fillWidth: true
        }
    }
}
