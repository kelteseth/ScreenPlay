import QtQuick
import QtQuick.Controls.Material

Item {
    property real customWidth: parent.width
    property real customHeight: 1
    property color customColor: Material.theme === Material.Light ? "#eeeeee" : Qt.darker(Material.foreground)
    property real customMargin: 10

    height: customMargin
    width: customWidth

    Rectangle {
        height: customHeight
        width: customWidth
        color: customColor
        opacity: Material.theme === Material.Light ? 1 : 0.2

        anchors {
            right: parent.right
            rightMargin: customMargin
            left: parent.left
            leftMargin: customMargin
            verticalCenter: parent.verticalCenter
        }

    }

}
