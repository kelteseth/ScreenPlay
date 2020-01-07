import QtQuick 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.2
import ScreenPlay 1.0

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

        Rectangle {
            id: leftWrapper
            color: "#333333"
            radius: 3
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * .5


        }
        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: 30

        }
        ColumnLayout {
            id: rightWrapper
            spacing: 8
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * .5
            Layout.alignment: Qt.AlignTop
            Text {
                text: qsTr("Type")
                font.pointSize: 14
                color: "#757575"
            }
            ComboBox {
                id: comboBox
                Layout.fillWidth: true
                model: ListModel {
                         id: model
                         ListElement { text: "QML" }
                         ListElement { text: "HTML" }
                     }
            }
            Text {
                text: qsTr("Settings")
                font.pointSize: 14
                color: "#757575"
            }
            TextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Widget name")
            }
            TextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Copyright owner")
            }
            Text {
                text: qsTr("License")
                font.pointSize: 14
                color: "#757575"
            }
            ComboBox {
                id: comboBoxLicense
                Layout.fillWidth: true
                model: ListModel {
                         id: modelLicense
                         ListElement { text: "All rights " }
                         ListElement { text: "Open Source - GPLv3" }
                         ListElement { text: "Open Source - MIT/Apache2" }
                     }
            }
            Text {
                text: qsTr("Help")
                font.pointSize: 14
                color: "#757575"
            }
            Row {
                Layout.fillWidth: true
                spacing: 70
                Button {
                    text: qsTr("QML Quickstart Guide")
                    Material.background: Material.Orange
                    Material.foreground: "white"
                    icon.source: "qrc:/assets/icons/icon_info.svg"
                    icon.color: "white"
                    icon.width: 16
                    icon.height: 16
                    onClicked: Qt.openUrlExternally(
                                   "http://qmlbook.github.io/ch04-qmlstart/qmlstart.html")
                }
                Button {
                    text: qsTr("Documentation")
                    Material.background: Material.LightGreen
                    Material.foreground: "white"
                    icon.source: "qrc:/assets/icons/icon_document.svg"
                    icon.color: "white"
                    icon.width: 16
                    icon.height: 16
                    onClicked: Qt.openUrlExternally(
                                   "https://kelteseth.gitlab.io/ScreenPlayDocs/")
                }
                Button {
                    text: qsTr("Forums")
                    Material.background: Material.Blue
                    Material.foreground: "white"
                    icon.source: "qrc:/assets/icons/icon_people.svg"
                    icon.color: "white"
                    icon.width: 16
                    icon.height: 16
                    onClicked: Qt.openUrlExternally(
                                   "https://forum.screen-play.app/")
                }
            }

        }
    }

    Row {
        height: 80
        width: childrenRect.width
        spacing: 10
        anchors {
            right: parent.right
            rightMargin: 30
            bottom: parent.bottom
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

        Button {
            id: btnSave
            text: qsTr("Save")
            enabled: false
            Material.background: Material.Orange
            Material.foreground: "white"

            onClicked: {
                savePopup.open()
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
            text: qsTr("Save Wallpaper...")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
        }

        Timer {
            id: timerSave
            interval: 3000 - Math.random() * 1000
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

