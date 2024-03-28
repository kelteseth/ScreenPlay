import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property int index: 0
    property bool selected: false
    property bool isLast: false
    property alias text: text.text

    signal remove(var index)
    signal lineSelected(var index)

    Text {
        id: text
        color: "white"
        //text: root.index
        anchors{
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10
        }
    }

    Rectangle {
        color: "gold"
        anchors.fill: parent
        visible: root.selected

    }

    Rectangle {
        id: indicatorLineVertical
        width: 5
        height: 30
        color:  root.selected ?  "gold"  : parent.color
        anchors{
            horizontalCenter: parent.horizontalCenter
            top: parent.bottom
            topMargin: 0
        }
    }

    Rectangle {
        id: background
        width: 70
        height: 48
        radius: 5
        border.width: root.selected ? 2 : 0
        border.color: "gold"
        color: parent.color
        anchors{
            horizontalCenter: parent.horizontalCenter
            top: indicatorLineVertical.bottom
            topMargin: -1
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked:{
                root.lineSelected(root.index)
            }
        }
    }
    ToolButton {
        text:"❌"
        visible: !root.isLast
        enabled: visible
        onClicked: root.remove(root.index)
        font.pointSize: 10


        anchors{
            left: background.right
            bottom: background.top
            margins: - 20
        }
    }
}
