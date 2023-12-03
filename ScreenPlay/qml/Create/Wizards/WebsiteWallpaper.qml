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
    function isValidURL(string) {
        var res = string.match(/(http(s)?:\/\/.)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)/g);
        return (res !== null);
    }
    sourceComponent: ColumnLayout {
        id: layout
        function validate() {
            root.ready = tfTitle.text.length >= 1 && root.isValidURL(tfUrl.text);
        }

        function create() {
            App.wizards.createWebsiteWallpaper(tfTitle.text, previewSelector.imageSource, tfUrl.text, tagSelector.getTags());
        }

        spacing: 10

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        Util.Headline {
            text: qsTr("Create a Website Wallpaper")
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
                onTextChanged: layout.validate()
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

        Util.TextField {
            id: tfUrl
            Layout.fillWidth: true
            required: true
            text: "https://"
            onTextChanged: layout.validate()
        }

        Item {
            height: 10
        }

        Util.HeadlineSection {
            text: qsTr("Tags")
        }

        Util.TagSelector {
            id: tagSelector

            Layout.fillWidth: true
        }

        Item {
            height: 10
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
