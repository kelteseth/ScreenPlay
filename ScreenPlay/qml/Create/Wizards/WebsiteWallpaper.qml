import QtQuick
import QtQuick.Layouts
import ScreenPlay

import ScreenPlayCore as SPCore

WizardPage {
    id: root
    function isValidURL(string) {
        var res = string.match(/(http(s)?:\/\/.)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)/g)
        return (res !== null)
    }
    sourceComponent: ColumnLayout {
        id: layout
        function validate() {
            root.ready = tfTitle.text.length >= 1 && root.isValidURL(tfUrl.text)
        }

        function create() {
            App.wizards.createWebsiteWallpaper(tfTitle.text, previewSelector.imageSource, tfUrl.text, tagSelector.getTags()).then(result => {
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
            text: qsTr("Create a Website Wallpaper")
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
                onTextChanged: layout.validate()
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

        SPCore.TextField {
            id: tfUrl
            Layout.fillWidth: true
            required: true
            text: "https://"
            onTextChanged: layout.validate()
        }

        Item {
            height: 10
        }

        SPCore.HeadlineSection {
            text: qsTr("Tags")
        }

        SPCore.TagSelector {
            id: tagSelector

            Layout.fillWidth: true
        }

        Item {
            height: 10
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
