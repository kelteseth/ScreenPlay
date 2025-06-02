import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Window
import QtQuick.Effects
import ScreenPlay
import ScreenPlayCore as Util

Util.Dialog {
    id: root
    property ApplicationWindow applicationWindow
    property string message
    standardButtons: Dialog.Ok | Dialog.Help
    onHelpRequested: {
        Qt.openUrlExternally("https://forum.screen-play.app/");
    }
    Connections {
        function onDisplayErrorPopup(msg: string): void {
            root.message = msg;
            root.applicationWindow.show();
            root.open();
        }
        target: App.screenPlayManager
    }
    contentItem: Item {
        implicitWidth: 600
        implicitHeight: 400
        ColumnLayout {
            anchors.margins: 20
            anchors.fill: parent
            spacing: 20
            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 150
                Layout.preferredHeight: 150
                source: "qrc:/qt/qml/ScreenPlay/assets/icons/exclamation-triangle-solid.svg"
                fillMode: Image.PreserveAspectFit
                layer {
                    enabled: true
                    effect: MultiEffect {
                        colorizationColor: Material.color(Material.DeepOrange)
                        colorization: 1.0
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
                font.family: App.settings.font
                font.pointSize: 16
                color: Material.primaryTextColor
            }
        }
    }
}
