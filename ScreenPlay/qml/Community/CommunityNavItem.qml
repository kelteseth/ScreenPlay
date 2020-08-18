import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import ScreenPlay 1.0

TabButton {
    id: control
    height: parent.height
    property url openLink

    contentItem: Item {
        anchors.fill: parent

        ToolButton {
            icon.source: control.icon.source
            anchors {
                right: txt.left
                verticalCenter: txt.verticalCenter
            }
            icon.color: control.checked ? Material.accentColor : Material.primaryTextColor
            hoverEnabled: false
            icon.width: 16
            icon.height: 16
            enabled: false
        }

        Text {
            id: txt
            text: control.text
            font.family: ScreenPlay.settings.font
            opacity: enabled ? 1.0 : 0.3
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
            anchors {
                top: parent.top
                topMargin: 15
                right: parent.right
            }
            opacity: 0.6

            width: parent.width * .2
            icon.source: "qrc:/assets/icons/icon_open_in_new.svg"
            icon.width: 16
            icon.height: 16
            onClicked: Qt.openUrlExternally(control.openLink)

            ToolTip.delay: 500
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Open in browser")
        }
    }

    background: Item {}
}

/*##^##
Designer {
    D{i:0;height:60;width:300}
}
##^##*/

