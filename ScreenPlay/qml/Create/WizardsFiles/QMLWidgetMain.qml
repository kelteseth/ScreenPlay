import QtQuick

Item {
    id: root
    implicitWidth: 300
    implicitHeight: 200

    Text {
        id: name
        text: qsTr("My Widget 🚀")
        anchors.centerIn: parent
    }
}
