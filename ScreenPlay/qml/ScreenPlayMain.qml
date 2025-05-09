import QtQml
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore
import QtCore as QCore
import "ContentSettings" as ContentSettings
import "Navigation" as Navigation

ApplicationWindow {
    id: applicationWindow
    color: Material.theme === Material.Dark ? Qt.darker(Material.backgroundColor) : Material.backgroundColor
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 810

    minimumHeight: 450
    minimumWidth: 1050

    Component.onCompleted: {
        applicationWindow.setTheme(App.settings.theme);
        if (!App.settings.silentStart) {
            App.showDockIcon(true);
            applicationWindow.show();
        }

        const isSteamVersion = App.globalVariables.isSteamVersion();
        let platform = "";
        if (isSteamVersion) {
            platform = qsTr("Steam");
        } else {
            platform = qsTr("Standalone");
        }
        let featureLevel = "";
        if (App.globalVariables.isProVersion()) {
            featureLevel = qsTr("Pro");
        } else if (App.globalVariables.isUltraVersion()) {
            featureLevel = qsTr("Ultra");
        } else {
            featureLevel = qsTr("Standard");
        }
        applicationWindow.title = "Open Source ScreenPlay v" + App.version() + " " + featureLevel + " " + platform;

        stackView.push("qrc:/qt/qml/ScreenPlay/qml/Installed/InstalledView.qml");
    }

    Connections {
        target: App.settings
        function onThemeChanged(theme) {
            applicationWindow.setTheme(theme);
        }
    }

    function setTheme(theme) {
        switch (theme) {
        case Settings.Theme.System:
            applicationWindow.Material.theme = Material.System;
            break;
        case Settings.Theme.Dark:
            applicationWindow.Material.theme = Material.Dark;
            break;
        case Settings.Theme.Light:
            applicationWindow.Material.theme = Material.Light;
            break;
        }
    }

    Connections {
        function onRequestRaise() {
            App.showDockIcon(true);
            applicationWindow.show();
        }
        target: App.screenPlayManager
    }

    Connections {
        target: App
        function onRequestExit() {
            crashOnExitWorkaround.forceActiveFocus();
            Qt.exit(0);
        }
    }
    FocusScope {
        id: crashOnExitWorkaround
    }

    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)
    onVisibilityChanged: {
        if (applicationWindow.visibility !== 2)
            return;
    }

    QCore.Settings {
        id: settings
    }

    onClosing: close => {
        close.accepted = false;
        if (App.screenPlayManager.activeWallpaperCounter === 0 && App.screenPlayManager.activeWidgetsCounter === 0) {
            App.exit();
        }
        const alwaysMinimize = settings.value("alwaysMinimize", null);
        if (alwaysMinimize === null) {
            console.error("Unable to retreive alwaysMinimize setting");
        }
        if (alwaysMinimize === "true") {
            applicationWindow.hide();
            App.showDockIcon(false);
            return;
        }
        content.openExitDialog();
    }

    Item {
        id: content
        anchors.fill: parent

        function openExitDialog(): void {
            exitDialog.open();
        }
        function switchPage(name: string): void {
            stackView.replace(`qrc:/qt/qml/ScreenPlay/qml/${name}/${name}View.qml`, {
                "modalSource": content
            });
            nav.setNavigation(name);
        }

        Navigation.ExitPopup {
            id: exitDialog
            applicationWindow: applicationWindow
            modalSource: content
            onAboutToShow: App.uiAppStateSignals.hideInstalledDrawer()
        }

        ContentSettings.ContentSettingsView {
            id: contentSettingsView
            modalSource: content
            onAboutToShow: App.uiAppStateSignals.hideInstalledDrawer()
        }

        TrayIcon {
            applicationWindow: applicationWindow
        }

        Connections {
            function onRequestNavigation(nav) {
                content.switchPage(nav);
            }

            target: App.uiAppStateSignals
        }

        StackView {
            id: stackView
            objectName: "stackView"
            property int duration: 300

            anchors {
                top: nav.bottom
                right: parent.right
                bottom: parent.bottom
                left: parent.left
            }

            replaceEnter: Transition {
                OpacityAnimator {
                    from: 0
                    to: 1
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }

                ScaleAnimator {
                    from: 0.8
                    to: 1
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }
            }

            replaceExit: Transition {
                OpacityAnimator {
                    from: 1
                    to: 0
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }

                ScaleAnimator {
                    from: 1
                    to: 0.8
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }
            }
        }
        Navigation.Navigation {
            id: nav
            modalSource: content
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            onChangePage: function (name) {
                // Close in case the user clicks
                // on the tray icon
                contentSettingsView.close();
                content.switchPage(name);
            }
        }
    }
}
