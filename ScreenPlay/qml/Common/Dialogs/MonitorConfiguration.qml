import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlay 1.0
import "../"

Dialog {
    id: dialogMonitorConfigurationChanged

    property var modalSource
    modal: true

    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok
    contentHeight: 250

    Overlay.modal: ModalBackgroundBlur {
        sourceItem: root.modalSource
    }

    Connections {
        function onMonitorConfigurationChanged() {
            dialogMonitorConfigurationChanged.open();
        }

        target: ScreenPlay.monitorListModel
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

}
 
