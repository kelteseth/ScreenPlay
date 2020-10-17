import QtQuick 2.14
import QtQuick.Controls.Material 2.12
import ScreenPlay 1.0

Item {
    id:root
    height: 40

    property alias text: txtHeadline.text

    Text {
        id: txtHeadline
        font.pointSize: 18
        color: Material.primaryTextColor
        text: qsTr("Headline")
        font.family: ScreenPlay.settings.font
    }

    Rectangle {
        height: 2
        width: parent.width
        color: Material.secondaryTextColor
        anchors {
            right:parent.right
            left:parent.left
            bottom: parent.bottom
        }
    }
}
