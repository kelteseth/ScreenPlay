import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlay
import ScreenPlayApp

Control {
    id: settingsComboBox

    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property bool enabled: true
    property alias comboBox: comboBox

    width: parent.width
    height: txtHeadline.paintedHeight + txtDescription.paintedHeight + 20

    Text {
        id: txtHeadline

        color: Material.foreground
        text: settingsComboBox.headline
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pointSize: 12
        font.family: App.settings.font

        anchors {
            top: parent.top
            topMargin: 6
            left: parent.left
            leftMargin: 20
        }
    }

    Text {
        id: txtDescription

        text: settingsComboBox.description
        color: Qt.darker(Material.foreground)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.WordWrap
        font.pointSize: 10
        font.family: App.settings.font

        anchors {
            top: txtHeadline.bottom
            topMargin: 6
            left: parent.left
            leftMargin: 20
            right: comboBox.left
            rightMargin: 20
        }
    }

    ComboBox {
        id: comboBox

        implicitWidth: 200
        textRole: "text"
        valueRole: "value"
        font.family: App.settings.font

        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }
    }
}
