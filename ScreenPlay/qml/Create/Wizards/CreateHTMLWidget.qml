import QtQuick 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.2
import ScreenPlay 1.0

import "../../Common"

Item {
    id: root
    function cleanup() {}

    signal wizardStarted
    signal wizardExited
    Headline {
        id: txtHeadline
        text: qsTr("Create an empty HTML widget")
        anchors {
            top: parent.top
            left: parent.left
            margins: 20
        }
    }
    RowLayout {
        anchors {
            top: txtHeadline.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
            margins: 20
        }

        ColumnLayout {
            Layout.preferredHeight: parent.width * .5
            Layout.preferredWidth: parent.width * .5

            Rectangle {
                id: leftWrapper
                color: "#333333"
                radius: 3
                Layout.fillHeight: true
                Layout.fillWidth: true

                Image {
                    id: imgPreview
                    source: "qrc:/assets/wizards/example_html.png"
                    width: parent.width
                    fillMode: Image.PreserveAspectCrop
                }
            }

            ImageSelector {
                id: imageSelector
                Layout.fillWidth: true
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: 20
        }
        ColumnLayout {
            id: rightWrapper
            spacing: 8
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * .5
            Layout.alignment: Qt.AlignTop
            Text {
                text: qsTr("General")
                font.pointSize: 14
                color: "#757575"
                font.family: ScreenPlay.settings.font
            }
            TextField {
                id: tfTitle
                Layout.fillWidth: true
                placeholderText: qsTr("Widget name")
                font.family: ScreenPlay.settings.font
                onTextChanged: {
                    if (text.length >= 3) {
                        btnSave.enabled = true
                    } else {
                        btnSave.enabled = false
                    }
                }
            }
            TextField {
                id: tfCreatedBy
                Layout.fillWidth: true
                placeholderText: qsTr("Copyright owner")
                font.family: ScreenPlay.settings.font
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

            TagSelector {
                id: tagSelector
                Layout.fillWidth: true
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
            }

            Row {
                height: 80
                layoutDirection: Qt.RightToLeft
                Layout.fillWidth: true
                spacing: 10

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
                        ScreenPlay.create.createWidget(
                                    ScreenPlay.globalVariables.localStoragePath,
                                    tfTitle.text, imageSelector.imageSource,
                                    tfCreatedBy.text, cbLicense.currentText,
                                    cbType.currentText, tags)
                    }
                }

                Connections {
                    target: ScreenPlay.create
                    function onWidgetCreatedSuccessful(path) {
                        ScreenPlay.util.openFolderInExplorer(path)
                    }
                }

                Button {
                    id: btnExit
                    text: qsTr("Abort")
                    font.family: ScreenPlay.settings.font
                    Material.background: Material.Red
                    Material.foreground: "white"
                    onClicked: {
                        root.wizardExited()
                    }
                }
            }
        }
    }

    Popup {
        id: savePopup
        modal: true
        focus: true
        width: 250
        anchors.centerIn: parent
        height: 200
        onOpened: timerSave.start()

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }
        Text {
            text: qsTr("Create Widget...")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
        }

        Timer {
            id: timerSave
            interval: 1000 + Math.random() * 1000
            onTriggered: {
                savePopup.close()
                root.wizardExited()
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:580;width:1200}
}
##^##*/

