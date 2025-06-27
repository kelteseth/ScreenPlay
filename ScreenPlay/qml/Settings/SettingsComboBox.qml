import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import ScreenPlay

Control {
    id: settingsComboBox

    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property bool enabled: true
    property bool proFeature: false
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

        text: {
            let baseText = settingsComboBox.description;
            if (settingsComboBox.proFeature && App.globalVariables.isBasicVersion()) {
                return baseText + " " + qsTr("(Pro feature - upgrade to unlock)");
            }
            return baseText;
        }
        color: Qt.darker(Material.foreground)
        opacity: (settingsComboBox.proFeature && App.globalVariables.isBasicVersion()) ? 0.5 : 1.0
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
            right: rowControls.left
            rightMargin: 20
        }
    }

    Row {
        id: rowControls
        spacing: 10
        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: comboBox

            implicitWidth: 200
            textRole: "text"
            valueRole: "value"
            font.family: App.settings.font
            enabled: settingsComboBox.enabled && !(settingsComboBox.proFeature && App.globalVariables.isBasicVersion())
            opacity: enabled ? 1.0 : 0.5
        }

        ToolButton {
            enabled: false
            visible: settingsComboBox.proFeature && App.globalVariables.isBasicVersion()
            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/font-awsome/lock-solid.svg"
            icon.width: 10
            icon.height: 10
            icon.color: "gold"
        }
    }
}
