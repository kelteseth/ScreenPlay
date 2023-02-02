import QtQuick
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlay

Item {
    id: root

    property alias text: txtHeadline.text

    height: 40

    Text {
        id: txtHeadline

        font.pointSize: 18
        color: Material.primaryTextColor
        text: "Headline"
        font.family: App.settings.font
    }

    Rectangle {
        height: 2
        width: parent.width
        color: Material.secondaryTextColor

        anchors {
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }
    }
}
