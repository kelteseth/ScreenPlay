import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Create
import ScreenPlayUtil as Util

WizardPage {
    id: root

    sourceComponent: ColumnLayout {
        property bool ready: tfTitle.text.length >= 1 && tfUrl.text.length > 1

        function create() {
            App.wizards.createWebsiteWallpaper(tfTitle.text, previewSelector.imageSource, tfUrl.text, tagSelector.getTags());
        }

        spacing: 10
        onReadyChanged: root.ready = ready

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

        Util.TextField {
            id: tfUrl

            Layout.fillWidth: true
            required: true
            text: "https://"
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
