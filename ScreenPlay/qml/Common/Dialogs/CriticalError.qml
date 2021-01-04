import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.2
import QtGraphicalEffects 1.15
import ScreenPlay 1.0

Dialog {
    id: root
    modal: true
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok | Dialog.Help
    onHelpRequested: {
        Qt.openUrlExternally(
                    "https://forum.screen-play.app/category/7/troubleshooting")
    }

    property Window mainWindow
    property string message

    Connections {
        target: ScreenPlay.screenPlayManager
        function onDisplayErrorPopup(msg) {
            root.message = msg
            root.mainWindow.show()
            root.open()
        }
    }

    contentItem: Item {
        width: 600
        height: 400
        ColumnLayout {

            anchors.margins: 20
            anchors.fill: parent
            spacing: 20

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 150
                Layout.preferredHeight: 150
                source: "qrc:/assets/icons/exclamation-triangle-solid.svg"
                fillMode: Image.PreserveAspectFit

                layer {
                    enabled: true
                    effect: ColorOverlay {
                        color: Material.color(Material.DeepOrange)
                    }
                }
            }

            Text {
                text: root.message
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 20
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: ScreenPlay.settings.font
                font.pointSize: 16
                color: Material.primaryTextColor
            }
        }
    }
}
