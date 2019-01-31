import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3
import net.aimber.create 1.0

Item {
    id: wrapperError
    opacity: 0
    
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
        color: Material.color(Material.Red)
        
        font.pixelSize: 32
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
}