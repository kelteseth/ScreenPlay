import QtQuick 2.14


Item {
    id:root
    width: parent.width
    height: 40

    property alias text: txtHeadline

    Text {
        id: txtHeadline
        font.family: "Roboto"
        font.pointSize: 18
        color: "#444444"
        text: qsTr("Headline")
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
