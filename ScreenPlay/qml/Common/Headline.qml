import QtQuick 2.14
import QtQuick.Controls.Material 2.12
import ScreenPlay 1.0

Item {
    id:root
    width: parent.width
    height: 40

    property alias text: txtHeadline

    Text {
        id: txtHeadline
        font.pointSize: 18
        color: "#444444"
        text: qsTr("Headline")
        font.family: ScreenPlay.settings.font
    }

    Rectangle {
        height: 2
        width: parent.width
        color: "#444444"
        anchors {
            right:parent.right
            left:parent.left
            bottom: parent.bottom
        }
    }
}
