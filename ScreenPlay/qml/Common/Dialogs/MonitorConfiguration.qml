import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.12

import ScreenPlay 1.0

Dialog {
    id: dialogMonitorConfigurationChanged
    modal: true
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok
    contentHeight: 250

    contentItem: Item {
        ColumnLayout {

            anchors.margins: 20
            anchors.fill: parent
            spacing: 20

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 150
                Layout.preferredHeight: 150
                source: "qrc:/assets/icons/monitor_setup.svg"
                fillMode: Image.PreserveAspectFit
            }

            Text {
                text: qsTr("Your monitor setup changed!\n Please configure your wallpaper again.")
                Layout.fillWidth: true
                Layout.fillHeight: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: ScreenPlay.settings.font
                font.pointSize: 16
                color: Material.primaryTextColor
            }
        }
    }
    Connections {
        target: ScreenPlay.monitorListModel
        function onMonitorConfigurationChanged() {
            dialogMonitorConfigurationChanged.open()
        }
    }
}
