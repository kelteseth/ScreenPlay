import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlayApp


Item {
    id: settingsButton

    property alias icon: btnSettings.icon
    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property string buttonText: "value"
    property bool isChecked: false
    property bool enabled: true
    property bool available: true

    signal buttonPressed

    height: txtHeadline.paintedHeight + txtDescription.paintedHeight + 20
    width: parent.width
    onAvailableChanged: {
        if (!available) {
            settingsButton.opacity = 0.5;
            btnSettings.enabled = false;
        } else {
            settingsButton.opacity = 1;
            btnSettings.enabled = true;
        }
    }

    Text {
        id: txtHeadline

        color: Material.foreground
        text: settingsButton.headline
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

        text: settingsButton.description
        color: Material.theme === Material.Light ? Qt.lighter(Material.foreground) : Qt.darker(Material.foreground)
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        font.pointSize: 10
        font.family: App.settings.font

        anchors {
            top: txtHeadline.bottom
            topMargin: 6
            left: parent.left
            leftMargin: 20
            right: btnSettings.left
            rightMargin: 20
        }
    }

    Button {
        id: btnSettings

        text: settingsButton.buttonText
        icon.width: 20
        icon.height: 20
        font.family: App.settings.font
        Material.background: Material.accent
        Material.foreground: "white"
        onPressed: buttonPressed()

        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }
    }
}
