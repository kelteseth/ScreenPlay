import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import ScreenPlayApp 1.0
import ScreenPlay 1.0


Item {
    id: root

    Rectangle {
        id: navWrapper

        color: Material.theme === Material.Light ? "white" : Material.background
        height: 46

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        RowLayout {
            anchors{
                fill: parent
                rightMargin: 20
                leftMargin: 20
            }

            spacing: 20
            Text {
                id: name
                text: qsTr("Download Wallpaper and Widgets from our forums manually. If you want to download Steam Workshop content you have to install ScreenPlay via Steam.")
                Layout.fillHeight: true
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 10
                color: Material.secondaryTextColor
            }
            Button {
                text: qsTr("Install Steam Version")
                onClicked: Qt.openUrlExternally("https://store.steampowered.com/app/672870/ScreenPlay/")
            }
            Button {
                text: qsTr("Open In Browser")
                onClicked: Qt.openUrlExternally("https://forum.screen-play.app/category/5/wallpaper")
            }
        }
    }

}
