import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

Item {
    id: settingsWrapper
    anchors.fill: parent


    ScrollView{

    }

    Column {
        width: 800
        height: parent.height
        spacing: 30
        //anchors.fill: parent
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
                    anchors {
                        top: headerGeneral.bottom
                        margins: 20
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                    }
                    spacing: 10
                    SettingBool {
                        headline: qsTr("Autostart")
                        description: qsTr("ScreenPlay will start with Windows and will setup your Desktop every time for you.")
                    }
                    SettingsHorizontalSeperator {
                    }
                    SettingBool {
                        headline: qsTr("High priority Autostart")
                        description: qsTr("This options grants ScreenPlay a higher autostart priority than other apps.")
                    }
                    SettingsHorizontalSeperator {
                    }
                    SettingsButton {
                        headline: qsTr("Set Save location")
                        description: qsTr("Choose where to find you content. The default save location is you steam installation")
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

                    }

                }
            }
        }
    }
}
