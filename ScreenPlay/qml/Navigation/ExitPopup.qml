import QtQuick
import Qt.labs.settings 1.0 as Labs
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import ScreenPlayUtil as Util
import Qt5Compat.GraphicalEffects

/*!
   \qmltype exitDialog
   \brief exitDialog

*/
Util.Popup {
    id: root
    property ApplicationWindow applicationWindow
    contentHeight: 400
    contentWidth: 600
    contentItem: Item {
        ColumnLayout {
            anchors.margins: 20
            anchors.fill: parent
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
                source: "qrc:/qml/ScreenPlayApp/assets/images/trayIcon_windows.png"
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

            Labs.Settings {
                id: settings
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Button {
                    text: qsTr("Quit ScreenPlay now")
                    onClicked: Qt.quit()
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
                        App.showDockIcon(false);
                        applicationWindow.hide();
                        root.close();
                    }
                }
            }
        }
    }
}
