import QtQuick 2.7

Rectangle {
    color: "steelblue"
    anchors.margins: 10

    property rect monitorSize: Qt.rect(0,0,0,0)
    property int index: 0
    property bool isSelected: false

    signal monitorSelected(var index)

    onIsSelectedChanged: {
        if(isSelected){
            color = "orange"
        } else {
            color = "steelblue"
        }
    }

    Column {
        spacing: 5

        Text {
            text: index + " - " + monitorSize.width + ", " + monitorSize.height
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            monitorSelected(index)
        }
    }
}
