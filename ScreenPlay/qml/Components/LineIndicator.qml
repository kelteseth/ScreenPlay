import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    z: selected ? 99 : 0
    property int index: 0
    property string identifier
    property bool selected: false // User selected
    property bool isActive: false // Active based on time
    property bool isLast: false
    property alias text: text.text

    signal remove(var index)
    signal lineSelected(var index)

    Text {
        id: text
        color: "white"
        //text: root.index
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10
        }
    }

    Rectangle {
        opacity: root.isActive ? 1 : 0
        color: "gold"
        height: root.height
        anchors {
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 200
            }
        }
    }

    Rectangle {
        visible: root.selected
        color: "gold"
        height: 3
        anchors {
            right: parent.right
            left: parent.left
            top: parent.bottom
        }
    }

    Rectangle {
        id: indicatorLineVertical
        width: 5
        height: 30
        color: root.selected ? "gold" : parent.color
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.bottom
            topMargin: 0
        }

        Behavior on color {

            ColorAnimation {
                duration: 200
            }
        }
    }

    Rectangle {
        id: monitorBackground
        width: 70
        height: 48
        radius: 5
        border.width: root.selected ? 2 : 0
        border.color: "gold"
        color: parent.color
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: indicatorLineVertical.bottom
            topMargin: -1
        }

        Behavior on color {

            ColorAnimation {
                duration: 200
            }
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                root.lineSelected(root.index);
            }
        }
        Text {
            anchors.centerIn: parent
            text: root.index + " - " + root.identifier
        }
    }
    ToolButton {
        text: "❌"
        visible: !root.isLast
        enabled: visible
        onClicked: root.remove(root.index)
        font.pointSize: 10

        anchors {
            left: monitorBackground.right
            bottom: monitorBackground.top
            margins: -20
        }
    }
}
