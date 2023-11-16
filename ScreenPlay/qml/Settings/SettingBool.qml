import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlay

Item {
    id: settingsBool

    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property bool isChecked: false
    property bool available: true

    signal checkboxChanged(bool checked)

    height: txtHeadline.paintedHeight + txtDescription.paintedHeight + 20
    width: parent.width
    onAvailableChanged: {
        if (!available) {
            settingsBool.opacity = 0.5;
            radioButton.enabled = false;
        } else {
            settingsButton.opacity = 1;
            radioButton.enabled = true;
        }
    }

    Text {
        id: txtHeadline

        color: Material.foreground
        text: settingsBool.headline
        font.family: App.settings.font
        font.pointSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

        anchors {
            top: parent.top
            topMargin: 6
            left: parent.left
            leftMargin: 20
            right: parent.right
            rightMargin: 20
        }
    }

    Text {
        id: txtDescription

        text: settingsBool.description
        wrapMode: Text.WordWrap
        linkColor: Material.color(Material.Orange)
        onLinkActivated: function (link) {
            Qt.openUrlExternally(link);
        }

        color: Material.theme === Material.Light ? Qt.lighter(Material.foreground) : Qt.darker(Material.foreground)
        font.family: App.settings.font
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pointSize: 10

        anchors {
            top: txtHeadline.bottom
            topMargin: 6
            left: parent.left
            leftMargin: 20
            right: radioButton.left
            rightMargin: 20
        }
    }

    CheckBox {
        id: radioButton

        checked: settingsBool.isChecked
        onClicked: {
            if (radioButton.checkState === Qt.Checked)
                checkboxChanged(true);
            else
                checkboxChanged(false);
        }

        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }
    }
}
