import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

CustomPage {
    pageName: ""

    Component.onCompleted: {
        steamWorkshop.searchWorkshop()
    }

    Connections{
        target: steamWorkshop
        onWorkshopSearched:{
            busyIndicator.running = false
            txtBusyIndicator.visible = false
        }

    }

    BusyIndicator {
        id: busyIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        running: true
        Material.accent: Material.Orange
    }

    Text {
        id: txtBusyIndicator
        text: qsTr("Searching for workshop content. Please stand by")
        anchors.top: busyIndicator.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
    }

}
