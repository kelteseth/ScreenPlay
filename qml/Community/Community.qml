import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2


Item {
    id: community
    anchors.fill: parent

    Item {
        id: communityWrapper
        width: 800
        height: 600

        anchors {
            top: parent.top
            topMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: qsTr("Open ScreenPlay Community")
            anchors.centerIn: parent
            Material.background: Material.Orange
            Material.foreground: "white"
            onClicked: {
                Qt.openUrlExternally("https://www.screen-play.rocks/")
            }
        }
    }

}
