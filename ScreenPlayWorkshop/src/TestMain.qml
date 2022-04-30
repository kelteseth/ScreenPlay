import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWorkshop 1.0

Window {
    id: root
    width: 1366
    height: 768
    visible: true
    title: qsTr("ScreenPlayWorkshop")

    Loader {
        anchors.fill: parent
        source:"qrc:/ScreenPlayWorkshop/qml/SteamWorkshop.qml"
    }
}
