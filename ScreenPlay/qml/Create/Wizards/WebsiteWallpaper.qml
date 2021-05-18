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
        property bool ready: tfTitle.text.length >= 1 && tfUrl.text.length > 1

        function create() {
            ScreenPlay.wizards.createWebsiteWallpaper(tfTitle.text, previewSelector.imageSource, tfUrl.text, tagSelector.getTags());
        }

        spacing: 10
        onReadyChanged: root.ready = ready

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        Common.Headline {
            text: qsTr("Create a Website Wallpaper")
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

        Common.TextField {
            id: tfUrl

            Layout.fillWidth: true
            required: true
            text: "https://"
        }

        Item {
            height: 10
        }

        Common.HeadlineSection {
            text: qsTr("Tags")
        }

        Common.TagSelector {
            id: tagSelector

            Layout.fillWidth: true
        }

        Item {
            height: 10
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
