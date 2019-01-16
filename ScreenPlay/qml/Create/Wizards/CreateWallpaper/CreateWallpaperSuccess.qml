import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3
import net.aimber.create 1.0

Item {
    id: wrapperSuccess
    opacity: 0

    Text {
        id: txtSuccessHeadline
        text: qsTr("Video creation success!")
        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }
        height: 40
        font.family: "Segoe UI, Roboto"
        font.weight: Font.Light
        color: Material.color(Material.Green)
        
        font.pixelSize: 32
    }

    AnimatedImage {
        id: imgSuccess
        asynchronous: true
        playing: true
        anchors.centerIn: parent
        width: 600
        height: 400
    }

    Button {
        id: btnSuccessBack
        text: qsTr("Back to create!")
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

/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
 ##^##*/
