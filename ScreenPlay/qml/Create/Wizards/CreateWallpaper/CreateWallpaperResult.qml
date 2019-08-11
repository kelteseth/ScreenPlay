import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3
import ScreenPlay.Create 1.0

Item {
    id: wrapperError

    property alias error:txtFFMPEGDebug.text

    Text {
        id: txtErrorHeadline
        text: qsTr("An error occurred!")
        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }
        height: 40
        font.family: "Segoe UI, Roboto"
        font.weight: Font.Light
        color: Material.color(Material.DeepOrange)
        font.pointSize: 32
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
            contentHeight: txtFFMPEGDebug.paintedHeight
            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                policy: ScrollBar.AlwaysOn
            }
            Text {
                id: txtFFMPEGDebug
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    margins: 20
                }
                wrapMode: Text.WordWrap
                color: "#626262"

                height: txtFFMPEGDebug.paintedHeight
            }
            Connections {
                target: screenPlayCreate
                onProcessOutput: {
                    txtFFMPEGDebug.text = text
                }
            }
        }
    }

    Button {
        id: btnBack
        text: qsTr("Back to create and send an error report!")
        Material.background: Material.Orange
        Material.foreground: "white"
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 10
        }
        onClicked: {
            utility.setNavigationActive(true)
            utility.setNavigation("Create")
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

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
