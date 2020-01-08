import QtQuick 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.2
import ScreenPlay 1.0

import "../../../Common"

Item {
    id: root
    function cleanup() {}

     Text {
        id: txtHeadline
        text: qsTr("Create an empty widget")
        height: 40
        font.family: "Roboto"
        font.weight: Font.Light
        color: "#757575"

        font.pointSize: 23
        anchors {
            top: parent.top
            left: parent.left
            margins: 40
            bottomMargin: 0
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

        ColumnLayout{
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * .5

            Rectangle {
                id: leftWrapper
                color: "#333333"
                radius: 3
                Layout.fillHeight: true
                Layout.fillWidth: true

            }

            ImageSelector {
                id:imageSelector
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
            }
            TextField {
                id:tfTitle
                Layout.fillWidth: true
                placeholderText: qsTr("Widget name")
                onTextChanged: {
                    if (text.length >= 3) {
                        btnSave.enabled = true
                    } else {
                        btnSave.enabled = false
                    }
                }
            }
            TextField {
                id:tfCreatedBy
                Layout.fillWidth: true
                placeholderText: qsTr("Copyright owner")
            }
            Text {
                text: qsTr("Type")
                font.pointSize: 14
                color: "#757575"
            }
            ComboBox {
                id: cbType
                Layout.fillWidth: true
                model: ListModel {
                    id: model
                    ListElement { text: "QML" }
                    ListElement { text: "HTML" }
                }
            }

            Text {
                text: qsTr("License")
                font.pointSize: 14
                color: "#757575"
            }
            ComboBox {
                id: cbLicense
                Layout.fillWidth: true
                model: ListModel {
                    id: modelLicense
                    ListElement { text: "All rights " }
                    ListElement { text: "Open Source - GPLv3" }
                    ListElement { text: "Open Source - MIT/Apache2" }
                }
            }
            Text {
                text: qsTr("Tags")
                font.pointSize: 14
                color: "#757575"
            }

            TagSelector {
                id:tagSelector
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
                    Material.background: Material.Orange
                    Material.foreground: "white"

                    onClicked: {
                        btnSave.enabled = false
                        savePopup.open()
                        var tags  = tagSelector.getTags()
                        ScreenPlay.create.createWidget(ScreenPlay.globalVariables.localStoragePath,
                                                     tfTitle.text,
                                                     imageSelector.imageSource,
                                                     tfCreatedBy.text,
                                                     cbLicense.currentText,
                                                     cbType.currentText,
                                                     tags)

                    }
                }

                Button {
                  id: btnExit
                  text: qsTr("Abort")
                  Material.background: Material.Red
                  Material.foreground: "white"
                  onClicked: {
                      ScreenPlay.util.setNavigationActive(true)
                      ScreenPlay.util.setNavigation("Create")
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
            interval: 1000 - Math.random() * 1000
            onTriggered: {
                ScreenPlay.util.setNavigationActive(true)
                ScreenPlay.util.setNavigation("Create")
            }
        }
    }



}

/*##^##
Designer {
    D{i:0;autoSize:true;height:580;width:1200}
}
##^##*/

