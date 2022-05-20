import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWorkshop

Window {
    id: root
    width: 1400
    height: 768
    visible: true
    title: qsTr("ScreenPlayWorkshop")
    Component.onCompleted: root.Material.theme = Material.Dark

    Loader {
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        source: "qrc:/qml/ScreenPlayWorkshop/qml/SteamWorkshop.qml"
    }
    Rectangle {
        height: 60
        color: Material.background
        id: nav
        anchors {
            right: parent.right
            left: parent.left
        }
    }

}
