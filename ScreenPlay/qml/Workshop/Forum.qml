import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import QtWebEngine 1.8
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
                font.family: ScreenPlay.settings.font
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

    WebEngineView {
        id: webView
        anchors {
            top: navWrapper.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        url:"https://forum.screen-play.app/category/5/wallpaper"

    }
}
