import QtQuick

Item {
    id: root
    // This is the default size of the widget.
    // These implicit width and height values are required!
    implicitWidth: 300
    implicitHeight: 200

    Rectangle {
        id: background
        anchors.fill: parent
        opacity: 0.9
        color: "#333333"
    }

    Text {
        id: text
        text: qsTr("My Widget 🚀")
        anchors.centerIn: parent
    }
}
