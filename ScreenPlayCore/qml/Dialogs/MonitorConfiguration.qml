import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlay

import ScreenPlayCore as Util

Util.Dialog {
    id: root

    standardButtons: Dialog.Ok
    contentHeight: 250

    Connections {
        function onMonitorConfigurationChanged()  : void {
            root.open();
        }

        target: App.monitorListModel
    }

    contentItem: Item {
        ColumnLayout {
            anchors.margins: 20
            anchors.fill: parent
            spacing: 20

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 150
                Layout.preferredHeight: 150
                source: "qrc:/qt/qml/ScreenPlay/assets/icons/monitor_setup.svg"
                fillMode: Image.PreserveAspectFit
            }

            Text {
                text: qsTr("Your monitor setup changed!\n Please configure your wallpaper again.")
                Layout.fillWidth: true
                Layout.fillHeight: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: App.settings.font
                font.pointSize: 16
                color: Material.primaryTextColor
            }
        }
    }
}
