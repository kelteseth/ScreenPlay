import QtQml
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay
import QtCore as QCore

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
        // applicationWindow.Material.theme = Material.Dark;
        // App is now a qml singleton to fix QtC autocompletion.
        // This also now means we have to make sure to init it here
        // and do _not_ access any other properties before we called init.
        App.init();
        setTheme(App.settings.theme);
        if (!App.settings.silentStart) {
            App.showDockIcon(true);
            applicationWindow.show();
        }
        baseLoader.setSource("qml/MainApp.qml", {
            "applicationWindow": applicationWindow
        });
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
        target: App
        function onRequestExit() {
            Qt.exit(0);
        }
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
        baseLoader.item.openExitDialog();
    }

    Loader {
        id: baseLoader
        asynchronous: true
        anchors.fill: parent
    }
}
