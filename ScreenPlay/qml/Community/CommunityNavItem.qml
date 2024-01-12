import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayApp


TabButton {
    id: control

    property url openLink

    height: parent.height
    onClicked: Qt.openUrlExternally(control.openLink)

    contentItem: Item {
        anchors.fill: parent

        ToolButton {
            icon.source: control.icon.source
            icon.color: control.checked ? Material.accentColor : Material.secondaryTextColor
            hoverEnabled: false
            icon.width: 16
            icon.height: 16
            enabled: false

            anchors {
                right: txt.left
                verticalCenter: txt.verticalCenter
            }
        }

        Text {
            id: txt

            text: control.text
            font.family: App.settings.font
            opacity: enabled ? 1 : 0.3
            color: control.checked ? Material.accentColor : Material.primaryTextColor
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            font.capitalization: Font.AllUppercase
            font.pointSize: 11
        }

        ToolButton {
            opacity: 0.6
            width: parent.width * 0.2
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_open_in_new.svg"
            icon.width: 16
            icon.height: 16
            onClicked: Qt.openUrlExternally(control.openLink)
            ToolTip.delay: 500
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Open in browser")

            anchors {
                top: parent.top
                topMargin: 15
                right: parent.right
            }
        }
    }

    background: Item {
    }
}
