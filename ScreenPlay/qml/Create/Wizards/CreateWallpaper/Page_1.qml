import QtQuick 2.9

Item {
    id: root
    Rectangle {
        id: rectangle1
        color: "gray"
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
    }

    Rectangle {
        id: rectangle
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }
}
