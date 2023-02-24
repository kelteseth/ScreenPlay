import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Particles

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
