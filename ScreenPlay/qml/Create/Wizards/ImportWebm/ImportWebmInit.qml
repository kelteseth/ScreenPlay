import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import ScreenPlayApp
import ScreenPlay

import ScreenPlayUtil as Util
import "../../"

Item {
    id: root

    signal next(var filePath)

    ColumnLayout {
        id: wrapper

        spacing: 40

        anchors {
            top: parent.top
            bottom: btnOpenDocs.top
            left: parent.left
            right: parent.right
            margins: 20
        }

        Util.Headline {
            Layout.fillWidth: true
            text: qsTr("Import a .webm video")
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 40

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 40

                Text {
                    id: txtDescription

                    text: qsTr("When importing webm we can skip the long conversion. When you get unsatisfying results with the ScreenPlay importer from 'ideo import and convert (all types)' you can also convert via the free and open source HandBrake!")
                    color: Material.primaryTextColor
                    Layout.fillWidth: true
                    font.pointSize: 13
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.family: App.settings.font
                }

                DropArea {
                    id: dropArea

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onExited: {
                        bg.color = Qt.darker(Material.backgroundColor);
                    }
                    onEntered: {
                        bg.color = Qt.darker(Qt.darker(Material.backgroundColor));
                        drag.accept(Qt.LinkAction);
                    }
                    onDropped: {
                        let file = App.util.toLocal(drop.urls[0]);
                        bg.color = Qt.darker(Qt.darker(Material.backgroundColor));
                        if (file.endsWith(".webm"))
                            root.next(drop.urls[0]);
                        else
                            txtFile.text = qsTr("Invalid file type. Must be valid VP8 or VP9 (*.webm)!");
                    }

                    Rectangle {
                        id: bg

                        anchors.fill: parent
                        radius: 3
                        color: Qt.darker(Material.backgroundColor)
                    }

                    Image {
                        id: bgPattern

                        anchors.fill: parent
                        fillMode: Image.Tile
                        opacity: 0.2
                        source: "qrc:/qml/ScreenPlayApp/assets/images/noisy-texture-3.png"
                    }

                    Text {
                        id: txtFile

                        text: qsTr("Drop a *.webm file here or use 'Select file' below.")
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        color: Material.primaryTextColor
                        font.pointSize: 13
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font.family: App.settings.font

                        anchors {
                            fill: parent
                            margins: 40
                        }
                    }
                }
            }
        }
    }

    Button {
        id: btnOpenDocs

        text: qsTr("Open Documentation")
        Material.accent: Material.color(Material.LightGreen)
        highlighted: true
        icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_document.svg"
        icon.color: "white"
        icon.width: 16
        icon.height: 16
        font.family: App.settings.font
        onClicked: Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/#performance")

        anchors {
            bottom: parent.bottom
            left: parent.left
            margins: 20
        }
    }

    Button {
        text: qsTr("Select file")
        highlighted: true
        font.family: App.settings.font
        onClicked: {
            fileDialogImportVideo.open();
        }

        FileDialog {
            id: fileDialogImportVideo

            nameFilters: ["Video files (*.webm)"]
            onAccepted: {
                root.next(fileDialogImportVideo.currentFile);
            }
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
    }
}
