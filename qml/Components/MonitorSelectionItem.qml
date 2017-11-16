import QtQuick 2.7

Rectangle {
    color: "steelblue"
    anchors.margins: 10

    property rect monitorSize: Qt.rect(0, 0, 0, 0)
    property string monitorModel
    property string monitorManufacturer
    property string monitorName
    property int index: 0
    property bool isSelected: false

    signal monitorSelected(var index)

    onIsSelectedChanged: {
        if (isSelected) {
            color = "orange"
        } else {
            color = "steelblue"
        }
    }

    Item {
        anchors.fill: parent
        Text {
            renderType: Text.NativeRendering
            text: index + monitorName + " " + monitorSize.width + ", " + monitorSize.height + " - " + monitorSize.x + ", " + monitorSize.y + " --" + monitorManufacturer + " - "  + monitorModel
            anchors.fill: parent
            color:"white"
            font.pixelSize: 8
            wrapMode:  Text.WrapAnywhere
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            monitorSelected(index)
        }
    }
}
