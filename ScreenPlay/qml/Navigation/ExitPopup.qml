import QtQuick
import QtQml
import QtCore as QCore
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp

import ScreenPlayUtil as Util
import Qt5Compat.GraphicalEffects

/*!
   \qmltype exitDialog
   \brief exitDialog

*/
Util.Popup {
    id: root
    property ApplicationWindow applicationWindow
    contentItem: Pane {
        background: Item {
        }
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
                        return "qrc:/qml/ScreenPlayApp/assets/images/trayIcon_windows.png";
                    }
                    if (Qt.platform.os === "osx") {
                        return "qrc:/qml/ScreenPlayApp/assets/images/trayIcon_osx.png";
                    }
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
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: App.settings.font
                font.pointSize: 12
                color: Material.primaryTextColor
            }

            QCore.Settings {
                id: settings
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
                        settings.setValue("alwaysMinimize", true);
                        settings.sync();
                        print(settings.value("alwaysMinimize"));
                        App.showDockIcon(false);
                        applicationWindow.hide();
                        root.close();
                    }
                }
            }
        }
    }
}
