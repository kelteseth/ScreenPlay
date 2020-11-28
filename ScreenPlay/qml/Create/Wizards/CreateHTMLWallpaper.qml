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
    sourceComponent: Item {

        Item {
            width: parent.width * .5
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 20
            }
            Image {
                source: "qrc:/assets/images/undraw_static_website_0107.svg"
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                }
                width: parent.width
                height: 400
                fillMode: Image.PreserveAspectFit
            }
        }
        Item {
            width: parent.width * .5
            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
                margins: 20
            }

            ColumnLayout {
                id: rightWrapper
                spacing: 8
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }

                Common.Headline {
                    text: qsTr("Create a html Wallpaper")
                }

                Text {
                    text: qsTr("General")
                    font.pointSize: 14
                    color: "#757575"
                    font.family: ScreenPlay.settings.font
                }
                Common.TextField {
                    id: tfTitle
                    Layout.fillWidth: true
                    placeholderText: qsTr("Wallpaper name")
                    onTextChanged: {
                        if (text.length >= 3) {
                            btnSave.enabled = true
                        } else {
                            btnSave.enabled = false
                        }
                    }
                }
                Common.TextField {
                    id: tfCreatedBy
                    Layout.fillWidth: true
                    placeholderText: qsTr("Copyright owner")
                }

                Text {
                    text: qsTr("License")
                    font.pointSize: 14
                    color: "#757575"
                    font.family: ScreenPlay.settings.font
                }
                ComboBox {
                    id: cbLicense
                    Layout.fillWidth: true
                    font.family: ScreenPlay.settings.font
                    model: ListModel {
                        id: modelLicense
                        ListElement {
                            text: "All rights "
                        }
                        ListElement {
                            text: "Open Source - GPLv3"
                        }
                        ListElement {
                            text: "Open Source - MIT/Apache2"
                        }
                    }
                }
                Text {
                    text: qsTr("Tags")
                    font.pointSize: 14
                    color: "#757575"
                    font.family: ScreenPlay.settings.font
                }

                Common.TagSelector {
                    id: tagSelector
                    Layout.fillWidth: true
                }
                Text {
                    text: qsTr("Preview Image")
                    font.pointSize: 14
                    color: "#757575"
                    font.family: ScreenPlay.settings.font
                }
                Common.ImageSelector {
                    id: previewSelector
                    placeHolderText: qsTr("You can set your own preview image here!")
                    Layout.fillWidth: true
                }
            }
        }
        Row {
            height: 80

            anchors {
                right: parent.right
                bottom: parent.bottom
                rightMargin: 20
            }

            spacing: 10

            Connections {
                target: ScreenPlay.create
                function onHtmlWallpaperCreatedSuccessful(path) {
                    ScreenPlay.util.openFolderInExplorer(path)
                }
            }

            Button {
                id: btnExit
                text: qsTr("Abort")
                Material.background: Material.Red
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                onClicked: {
                    root.decrementCurrentIndex()
                    root.wizardExited()
                }
            }

            Button {
                id: btnSave
                text: qsTr("Save")
                enabled: false
                Material.background: Material.accent
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                onClicked: {
                    btnSave.enabled = false
                    savePopup.open()
                    var tags = tagSelector.getTags()
                    ScreenPlay.create.createHTMLWallpaper(
                                ScreenPlay.globalVariables.localStoragePath,
                                tfTitle.text, previewSelector.imageSource,
                                cbLicense.currentText, tags)
                }
            }
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

