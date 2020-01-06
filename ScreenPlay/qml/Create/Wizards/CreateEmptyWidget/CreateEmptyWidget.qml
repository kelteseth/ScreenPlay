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

        Item {
            id: leftWrapper
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * .5

            TextField {
                Layout.fillWidth: true
            }
        }
        ColumnLayout {
            id: rightWrapper
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * .5

            TextField {
                Layout.fillWidth: true
            }
            TextField {
                Layout.fillWidth: true
            }
            TextField {
                Layout.fillWidth: true
            }
            TextField {
                Layout.fillWidth: true
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

