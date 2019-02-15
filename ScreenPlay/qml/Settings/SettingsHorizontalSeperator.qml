import QtQuick 2.12

Item {
    property real customWidth: parent.width
    property real customHeight: 1
    property color customColor: "#C3C3C3"
    property real customMargin: 10

    height: customMargin
    width:customWidth

    Rectangle {
        height: customHeight
        width: customWidth
        color: customColor
        anchors {
            right: parent.right
            rightMargin: customMargin
            left: parent.left
            leftMargin:customMargin

        }
    }
}
