import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common" as Common

Item {
    id: root
    signal next(var filePath)

    ColumnLayout {
        spacing: 40

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 20
        }

        Common.Headline {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            text: qsTr("Import a video")
        }

        Text {
            id: txtDescription
            text: qsTr("When importing webm we can skip the long conversion.")
            color: Material.primaryTextColor
            Layout.fillWidth: true
            font.pointSize: 13
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.family: ScreenPlay.settings.font
        }

    }

    Button {
        text: qsTr("Open Documentation")
        Material.background: Material.LightGreen
        Material.foreground: "white"
        icon.source: "qrc:/assets/icons/icon_document.svg"
        icon.color: "white"
        icon.width: 16
        icon.height: 16
        font.family: ScreenPlay.settings.font
        onClicked: Qt.openUrlExternally(
                       "https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/#performance")
        anchors {
            bottom: parent.bottom
            margins: 20
        }
    }
    Button {
        text: qsTr("Select file")
        highlighted: true
        font.family: ScreenPlay.settings.font
        onClicked: {
            fileDialogImportVideo.open()
        }

        FileDialog {
            id: fileDialogImportVideo
            nameFilters: ["Video files (*.webm)"]

            onAccepted: {
                root.next(fileDialogImportVideo.fileUrl)
            }
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
##^##*/

