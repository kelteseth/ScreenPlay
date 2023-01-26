import QtQuick

Item {
    id: root
    implicitWidth: 300
    implicitHeight: 200

    Rectangle {
        id: background
        anchors.fill:parent
        opacity: 0.9
        color: "#333333"
    }

    Text {
        id: text
        text: qsTr("My Widget 🚀")
        anchors.centerIn: parent
    }
}
