import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import ScreenPlayUtil as Util

WizardPage {
    id: root

    sourceComponent: ColumnLayout {
        id: rightWrapper

        function create() {
            App.wizards.createGodotWallpaper(tfTitle.text, cbLicense.name,
                                             cbLicense.licenseFile,
                                             tfCreatedBy.text,
                                             previewSelector.imageSource,
                                             tagSelector.getTags()).then(
                        result => {
                            wizardFinished(result.success, result.message)
                        })
        }

        spacing: 10

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        Util.Headline {
            text: qsTr("Create a Godot Wallpaper")
            Layout.fillWidth: true
        }

        Util.HeadlineSection {
            text: qsTr("General")
        }

        RowLayout {
            spacing: 20

            Util.TextField {
                id: tfTitle

                Layout.fillWidth: true
                placeholderText: qsTr("Wallpaper name")
                required: true
                onTextChanged: root.ready = text.length >= 1
            }

            Util.TextField {
                id: tfCreatedBy

                Layout.fillWidth: true
                placeholderText: qsTr("Created By")
            }
        }

        Util.TextField {
            id: tfDescription

            Layout.fillWidth: true
            placeholderText: qsTr("Description")
        }

        Item {
            height: 30
        }

        Util.HeadlineSection {
            text: qsTr("License & Tags")
        }

        RowLayout {
            spacing: 20

            Util.LicenseSelector {
                id: cbLicense
            }

            Util.TagSelector {
                id: tagSelector

                Layout.fillWidth: true
            }
        }

        Item {
            height: 30
        }

        Util.HeadlineSection {
            text: qsTr("Preview Image")
        }

        Util.ImageSelector {
            id: previewSelector

            Layout.fillWidth: true
        }
    }
}
