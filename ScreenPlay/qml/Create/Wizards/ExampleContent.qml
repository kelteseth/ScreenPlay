pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore as Util

FocusScope {
    id: root

    property bool showSaveButton: false

    // Signals to maintain compatibility with wizard system
    signal wizardStarted
    signal wizardExited
    signal wizardFinished(bool success, string message)
    signal saveClicked
    signal saveFinished
    signal cancelClicked

    ColumnLayout {
        id: rightWrapper

        function addToCollection(example) {
            App.wizards.copyExampleContent(example.folderName).then(result => {
                if (result.success) {
                    snackBar.show(qsTr("Successfully added '%1' to your collection!").arg(example.title));
                    // Reload the installed list model to show the new content
                    App.installedListModel.reset();
                } else {
                    snackBar.show(result.message || qsTr("Failed to add to collection"));
                }
            });
        }

        spacing: 10

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            bottom: parent.bottom
            margins: 20
        }

        Util.Headline {
            text: qsTr("Install Example Content")
            Layout.fillWidth: true
        }

        Text {
            text: qsTr("Choose from a variety of example wallpapers and widgets to install in your collection.")
            font.pointSize: 11
            color: Material.secondaryTextColor
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: App.wizards.getExampleContent()
            spacing: 10
            focus: true
            interactive: true
            clip: true

            section.property: "category"
            section.criteria: ViewSection.FullString
            section.delegate: Rectangle {
                id: sectionDelegate
                width: listView.width
                height: 40
                color: Material.theme === Material.Light ? "#f5f5f5" : "#2a2a2a"

                required property string section

                Text {
                    text: sectionDelegate.section
                    font.pointSize: 14
                    font.bold: true
                    color: Material.primaryTextColor
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 15
                }
            }

            delegate: Rectangle {
                id: delegateItem
                width: listView.width
                height: Math.max(120, contentLayout.implicitHeight + 20)
                color: "transparent"
                border.color: "transparent"
                border.width: 1
                radius: 8

                required property var model

                RowLayout {
                    id: contentLayout
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15

                    // Preview image
                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 80
                        color: Material.backgroundDimColor
                        radius: 6

                        Image {
                            anchors.fill: parent
                            source: delegateItem.model.preview
                            fillMode: Image.PreserveAspectCrop

                            Rectangle {
                                anchors.fill: parent
                                color: "transparent"
                                border.color: Material.dividerColor
                                border.width: 1
                                radius: 6
                            }
                        }
                    }

                    // Content info
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.maximumHeight: delegateItem.height - 20
                        spacing: 5

                        Text {
                            text: delegateItem.model.title
                            font.pointSize: 14
                            font.bold: true
                            color: Material.primaryTextColor
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Text {
                            text: delegateItem.model.description || qsTr("No description available")
                            font.pointSize: 10
                            color: Material.secondaryTextColor
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        // Tags
                        Flow {
                            Layout.fillWidth: true
                            spacing: 5

                            Repeater {
                                model: delegateItem.model.tags

                                delegate: Rectangle {
                                    required property var modelData

                                    width: tagText.width + 16
                                    height: 20
                                    color: Material.color(Material.Grey)
                                    radius: 10

                                    Text {
                                        id: tagText
                                        anchors.centerIn: parent
                                        text: parent.modelData
                                        font.pointSize: 8
                                        color: "white"
                                    }
                                }
                            }
                        }
                    }

                    // Add to Collection button
                    Button {
                        Layout.minimumWidth: 120
                        Layout.preferredWidth: implicitWidth + 20
                        Layout.preferredHeight: 40
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("Add to Collection")
                        onClicked: rightWrapper.addToCollection(delegateItem.model)
                    }
                }
            }
        }
    }

    // Snackbar for success/error messages
    Util.MaterialSnackBar {
        id: snackBar
    }
}
