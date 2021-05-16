import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import ScreenPlay 1.0
import ScreenPlay.Create 1.0

Item {
    id: wrapperError

    property alias error: txtFFMPEGDebug.text

    Text {
        id: txtErrorHeadline

        text: qsTr("An error occurred!")
        height: 40
        font.family: ScreenPlay.settings.font
        font.weight: Font.Light
        color: Material.color(Material.DeepOrange)
        font.pointSize: 32

        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }

    }

    Rectangle {
        id: rectangle1

        color: "#eeeeee"
        radius: 3

        anchors {
            top: txtErrorHeadline.bottom
            right: parent.right
            bottom: btnBack.top
            left: parent.left
            margins: 30
            bottomMargin: 10
        }

        Flickable {
            anchors.fill: parent
            clip: true
            contentHeight: txtFFMPEGDebug.paintedHeight + 100

            Text {
                id: txtFFMPEGDebug

                font.family: ScreenPlay.settings.font
                wrapMode: Text.WordWrap
                color: "#626262"
                text: ScreenPlay.create.ffmpegOutput
                height: txtFFMPEGDebug.paintedHeight

                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    margins: 20
                }

            }

            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                acceptedButtons: Qt.RightButton
                onClicked: contextMenu.popup()
            }

            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                policy: ScrollBar.AlwaysOn
            }

        }

    }

    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Copy text to clipboard")
            onClicked: {
                ScreenPlay.util.copyToClipboard(txtFFMPEGDebug.text);
            }
        }

    }

    Button {
        id: btnBack

        text: qsTr("Back to create and send an error report!")
        Material.background: Material.accent
        Material.foreground: "white"
        font.family: ScreenPlay.settings.font
        onClicked: {
            ScreenPlay.util.setNavigationActive(true);
            ScreenPlay.util.setNavigation("Create");
        }

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 10
        }

    }

    states: [
        State {
            name: "error"

            PropertyChanges {
                target: txtFFMPEGDebug
                text: "Error!"
            }

        },
        State {
            name: "success"

            PropertyChanges {
                target: txtFFMPEGDebug
                text: "Success!"
            }

        }
    ]
}
