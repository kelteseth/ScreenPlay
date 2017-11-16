import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import "Settings/"

CustomPage {
    id: settingsWrapper
    pageName: ""

    VisualItemModel {
        id: settingsModel
/*
        SettingsWrapper {
            height: 180
            Column {
                spacing: 10
                anchors.margins: 30

                anchors.fill: parent
                Text {
                    text: "General"
                    height: 50
                    width: parent.width
                }
                SettingBool {
                    name: "Autostart"
                    height: 50
                    width: parent.width
                    isChecked: screenPlaySettings.autostart
                    onCheckboxChanged: screenPlaySettings.setAutostart(
                                           isChecked)
                }
            }
        }*/
        SettingsWrapper {
            height: 180
            Column {
                spacing: 10
                anchors.margins: 30
                anchors.fill: parent

                Headline {
                    name: "Save"
                    height: 20
                    width: parent.width
                }
                Row {
                    height: 100
                    width: parent.width
                    spacing: 30
                    Text {
                        id: txtSavePathDescription
                        text: qsTr("Save Path: ")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        id: txtSavePath
                        anchors.verticalCenter: parent.verticalCenter
                        text: screenPlaySettings.localStoragePath
                    }
                    Button {
                        id: btnSavePath
                        text: qsTr("Set Save Location")
                        onClicked: fileDialogOpenSavePath.open()
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    FileDialog {
                        id: fileDialogOpenSavePath
                        folder: shortcuts.home
                        selectFolder: true

                        onAccepted: {
                            txtSavePath.text = fileDialogOpenSavePath.folder
                            screenPlaySettings.setLocalStoragePath(
                                        fileDialogOpenSavePath.folder)
                        }
                    }/**/
                }
            }
        }
    }

    ListView {
        id: settingsListView
        anchors {
            fill: parent
            bottomMargin: 30
            leftMargin: 30
        }
        header: Item {
            height: 10
            width: parent.width
        }
        spacing: 30
        model: settingsModel

        cacheBuffer: 100
        maximumFlickVelocity: 1000
        flickDeceleration: 500
        boundsBehavior: Flickable.DragOverBounds
        ScrollBar.vertical: ScrollBar {
        }
        displaced: Transition {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1.0
                duration: 400
            }
        }
    }
}
