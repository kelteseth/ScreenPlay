import QtQuick
import QtQuick.Controls.Material
import ScreenPlayApp 1.0
import ScreenPlay 1.0

Text {
    text: qsTr("Headline Section")
    font.pointSize: 14
    color: Material.primaryTextColor
    font.family: App.settings.font
}
