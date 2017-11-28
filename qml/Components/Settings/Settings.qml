import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

import "Settings/"

Item {
    id: settingsWrapper
    anchors.fill: parent
    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    Item {
        height: 180
        width: 800
        anchors {
            top: parent.top
            topMargin: 40
            horizontalCenter: parent.horizontalCenter
        }

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
            Item {
                anchors {
                    fill: parent
                    margins: 20
                }
                Text {
                    id: txtheadlineGeneral
                    height: 20
                    text: qsTr("General")
                    font.pointSize: 11
                    color: "#3a3a3a"
                    renderType: Text.NativeRendering
                    font.family: font_Roboto_Regular.name
                }
                Row {
                    anchors {
                        top: txtheadlineGeneral.bottom
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                    }

                    spacing: 30
                    Text {
                        id: txtSavePathDescription
                        text: qsTr("Save Path: ")
                        anchors.verticalCenter: parent.verticalCenter
                        font.pointSize: 11
                        color: "#818181"
                        renderType: Text.NativeRendering
                        font.family: font_Roboto_Regular.name
                    }
                    Text {
                        id: txtSavePath
                        anchors.verticalCenter: parent.verticalCenter
                        text: screenPlaySettings.localStoragePath
                        font.pointSize: 11
                        color: "#818181"
                        renderType: Text.NativeRendering
                        font.family: font_Roboto_Regular.name
                    }
                    Button {
                        id: btnSavePath
                        text: qsTr("Set Save Location")
                        onClicked: fileDialogOpenSavePath.open()
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    FolderDialog {
                        id: fileDialogOpenSavePath
                        onAccepted: {
                            txtSavePath.text = fileDialogOpenSavePath.folder
                            screenPlaySettings.setLocalStoragePath(
                                        fileDialogOpenSavePath.folder)
                        }
                    }
                }
            }
        }
    }
}
