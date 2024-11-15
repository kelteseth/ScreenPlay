import QtQml
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayApp 1.0
import QtCore as QCore

ApplicationWindow {
    id: root
    color: Material.theme === Material.Dark ? Qt.darker(Material.background) : Material.background
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 810

    minimumHeight: 450
    minimumWidth: 1050
    Component.onCompleted: {
        // App is now a qml singleton to fix QtC autocompletion.
        // This also now means we have to make sure to init it here
        // and do _not_ access any other properties before we called init.
        App.init();
        if (!App.settings.silentStart) {
            App.showDockIcon(true);
            root.show();
        }
        baseLoader.setSource("qrc:/qml/ScreenPlayApp/qml/MainApp.qml");
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
        root.title = "ScreenPlay v" + App.version() + " " + featureLevel + " " + platform;
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
        if (root.visibility !== 2)
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
            root.hide();
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
