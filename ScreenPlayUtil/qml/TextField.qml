import QtQuick
import QtQuick.Window
import QtQuick.Controls.Material
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlayApp


ColumnLayout {
    id: root

    implicitHeight: 70
    implicitWidth: 150
    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    property bool required: false

    TextField {
        id: textField
        Layout.fillWidth: true
    }

    Text {
        id: requiredText
        text: root.required ? qsTr("*Required") : ""
        font.family: App.settings.font
        color: Material.secondaryTextColor
        Layout.alignment: Qt.AlignRight
    }
}
