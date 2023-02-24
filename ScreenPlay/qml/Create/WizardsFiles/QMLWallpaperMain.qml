import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Particles


Rectangle {
    id: root
    color: "#333333"
    // This is the default testing size of the wallpaper
    implicitWidth: 1366
    implicitHeight: 768

    Text {
        id: name
        text: qsTr("My ScreenPlay Wallpaper 🚀")
        anchors.centerIn: parent
        color: "white"
    }
}
