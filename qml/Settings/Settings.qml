import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

Item {
    id: settingsWrapper
    anchors.fill: parent

    Connections {
        target: screenPlaySettings
        onDecoderChanged:{
            if(decoder === "CUDA"){
                settingsComboBox.currentIndex = 0
            } else if(decoder === "D3D11"){
                settingsComboBox.currentIndex = 1
            }else if(decoder === "DXVA"){
                settingsComboBox.currentIndex = 2
            }else if(decoder === "VAAPI"){
                settingsComboBox.currentIndex = 3
            }else if(decoder === "FFmpeg"){
                settingsComboBox.currentIndex = 4
            }
        }
    }

    Column {
        width: 800
        height: parent.height
        spacing: 30
        anchors {
            top: parent.top
            topMargin: 40
            horizontalCenter: parent.horizontalCenter
        }
        Item {
            id: settingsGeneralWrapper
            height: 320
            width: 800

            RectangularGlow {
                id: effectBtnEmpty
                anchors {
                    top: parent.top
                }

                height: parent.height
                width: parent.width
                cached: true
                glowRadius: 3
                spread: 0.2
                color: "black"
                opacity: 0.2
                cornerRadius: 15
            }

            Rectangle {
                anchors.fill: parent
                radius: 4
                clip: true

                SettingsHeader {
                    id: headerGeneral
                    text: qsTr("General")
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                }

                Column {
                    spacing: 10
                    anchors {
                        top: headerGeneral.bottom
                        margins: 20
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                    }
                    SettingBool {
                        headline: qsTr("Autostart")
                        description: qsTr("ScreenPlay will start with Windows and will setup your Desktop every time for you.")
                        isChecked: screenPlaySettings.autostart
                        onCheckboxChanged: {
                            screenPlaySettings.setAutostart(checked)
                            screenPlaySettings.writeSingleSettingConfig("autostart",checked)
                        }

                    }
                    SettingsHorizontalSeperator {
                    }
                    SettingBool {
                        headline: qsTr("High priority Autostart")
                        description: qsTr("This options grants ScreenPlay a higher autostart priority than other apps.")
                        isChecked: screenPlaySettings.highPriorityStart
                        onCheckboxChanged: {
                            screenPlaySettings.setHighPriorityStart(checked)
                            screenPlaySettings.writeSingleSettingConfig("highPriorityStart",checked)
                        }
                    }
                    SettingsHorizontalSeperator {
                    }
                    SettingsButton {
                        headline: qsTr("Set save location")
                        description: screenPlaySettings.localStoragePath //qsTr("Choose where to find you content. The default save location is you steam installation")
                        buttonText: qsTr("Set save location")
                        onButtonPressed: {
                            folderDialogSaveLocation.open()
                        }
                        FolderDialog {
                            id: folderDialogSaveLocation
                            onAccepted: {
                                screenPlaySettings.setLocalStoragePath(folderDialogSaveLocation.currentFolder)
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: settingsPerformanceWrapper
            height: 150
            width: 800


            RectangularGlow {
                id: effect2
                anchors {
                    top: parent.top
                }

                height: parent.height
                width: parent.width
                cached: true
                glowRadius: 3
                spread: 0.2
                color: "black"
                opacity: 0.2
                cornerRadius: 15
            }

            Rectangle {
                anchors.fill: parent
                radius: 4
                clip: true

                SettingsHeader {
                    id: headerPerformance
                    text: qsTr("Performance")
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                }

                Column {
                    anchors {
                        top: headerPerformance.bottom
                        margins: 20
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                    }
                    spacing: 10


                    SettingsComboBox {
                        id:settingsComboBox
                        headline: qsTr("Set decoder")
                        description: qsTr("ScreenPlay supports different encoders for different hardware requirements.")
                        onCurrentIndexChanged: {
                            screenPlaySettings.setDecoder(settingsComboBox.comboBoxListModel.get(settingsComboBox.currentIndex).text.toString())
                        }
                        comboBoxListModel: ListModel {
                            id: model
                            ListElement { text: "CUDA" }
                            ListElement { text: "D3D11" }
                            ListElement { text: "DXVA" }
                            ListElement { text: "VAAPI" }
                            ListElement { text: "FFmpeg" }
                        }
                    }

                }
            }
        }
    }
}
