import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12
import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

Item {
    id: root
    anchors.fill: parent

    Text {
        color: Material.primaryTextColor
        anchors {
            top: parent.top
            right: parent.right
            bottom: footer.top
            left: parent.left
            margins: 20
        }
        font.pointSize: 14
        font.family: ScreenPlay.settings.font
        text: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor.
                Aenean massa. \n\n\nCum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. "
    }

    Row {
        id: footer
        height: 50
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }

        spacing: 20
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
            onClicked: Qt.openUrlExternally("https://forum.screen-play.app/")
        }
        Button {
            text: qsTr("Workshop")
            Material.background: Material.Red
            Material.foreground: "white"

            icon.source: "qrc:/assets/icons/icon_steam.svg"
            icon.color: "white"
            icon.width: 16
            icon.height: 16
            onClicked: Qt.openUrlExternally(
                           "steam://url/SteamWorkshopPage/672870")
        }
    }
}
