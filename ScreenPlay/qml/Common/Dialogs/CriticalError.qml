import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import ScreenPlay 1.0

Dialog {
    id: root

    property Window mainWindow
    property string message

    modal: true
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok | Dialog.Help
    onHelpRequested: {
        Qt.openUrlExternally("https://forum.screen-play.app/category/7/troubleshooting");
    }

    Connections {
        function onDisplayErrorPopup(msg) {
            root.message = msg;
            root.mainWindow.show();
            root.open();
        }

        target: ScreenPlay.screenPlayManager
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
