import QtQuick
import QtQml
import QtCore as QCore
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay
import ScreenPlayCore as Util

Popup {
    id: root
    property Item modalSource
    property ApplicationWindow applicationWindow
    anchors.centerIn: root.modalSource
    onAboutToHide: {
        // Fixes the ugly transition with
        // our ModalBackgroundBlur on exit
        modal = false;
    }
    onAboutToShow: {
        modal = true;
    }
    modal: true

    Overlay.modal: Util.ModalBackgroundBlur {
        id: blurBg
        sourceItem: root.modalSource
    }

    contentItem: Pane {
        background: Item {}
        padding: 20
        bottomPadding: 10
        ColumnLayout {
            spacing: 20

            Text {
                text: qsTr("You have active Wallpaper.\nScreenPlay will only quit if no Wallpaper are running.")
                Layout.fillWidth: true
                Layout.fillHeight: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: App.settings.font
                font.pointSize: 16
                color: Material.primaryTextColor
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                source: {
                    if (Qt.platform.os === "windows") {
                        return "qrc:/qt/qml/ScreenPlay/assets/images/trayIcon_windows.png";
                    }
                    if (Qt.platform.os === "osx") {
                        return "qrc:/qt/qml/ScreenPlay/assets/images/trayIcon_osx.png";
                    }
                    console.warn("missing tray icon image");
                    return "";
                }

                fillMode: Image.PreserveAspectFit
            }

            Text {
                text: {
                    if (Qt.platform.os === "windows") {
                        return qsTr("You can <b>quit</b> ScreenPlay via the bottom right Tray-Icon.");
                    }
                    if (Qt.platform.os === "osx") {
                        return qsTr("You can <b>quit</b> ScreenPlay via the top right Tray-Icon.");
                    }
                    return qsTr("You can <b>quit</b> ScreenPlay via the Tray-Icon.");
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: App.settings.font
                font.pointSize: 12
                color: Material.primaryTextColor
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Button {
                    text: qsTr("Quit ScreenPlay now")
                    onClicked: App.exit()
                }
                Button {
                    text: qsTr("Minimize ScreenPlay")
                    onClicked: {
                        applicationWindow.hide();
                        App.showDockIcon(false);
                        root.close();
                    }
                }
                Button {
                    highlighted: true
                    text: qsTr("Always minimize ScreenPlay")
                    onClicked: {
                        App.settings.setAlwaysMinimize(true);
                        App.showDockIcon(false);
                        applicationWindow.hide();
                        root.close();
                    }
                }
            }
        }
    }
}
