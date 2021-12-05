import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay 1.0
import Settings 1.0
import Qt5Compat.GraphicalEffects
import "qml/Monitors" as Monitors
import "qml/Common" as Common
import "qml/Common/Dialogs" as Dialogs
import "qml/Installed" as Installed
import "qml/Navigation" as Navigation
import "qml/Workshop" as Workshop
import "qml/Community" as Community

ApplicationWindow {
    id: root

    function setTheme(theme) {
        switch (theme) {
        case Settings.System:
            root.Material.theme = Material.System;
            break;
        case Settings.Dark:
            root.Material.theme = Material.Dark;
            break;
        case Settings.Light:
            root.Material.theme = Material.Light;
            break;
        }
    }

    function switchPage(name) {
        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                ScreenPlay.installedListModel.reset()
        }

        if (name === "Installed") {
            stackView.replace("qrc:/ScreenPlay/qml/" + name + "/" + name + ".qml", {
                                  "sidebar": sidebar
                              })
            return
        }
        stackView.replace("qrc:/ScreenPlay/qml/" + name + "/" + name + ".qml")
        sidebar.state = "inactive"
    }

    color: Material.theme === Material.Dark ? Qt.darker(Material.background) : Material.background
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 788
    title: "ScreenPlay Alpha - " + ScreenPlay.version()
    minimumHeight: 450
    minimumWidth: 1050
    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)
    onVisibilityChanged: {
        if (root.visibility === 2)
            ScreenPlay.installedListModel.reset();

    }
    onClosing: {
        if (ScreenPlay.screenPlayManager.activeWallpaperCounter === 0
                && ScreenPlay.screenPlayManager.activeWidgetsCounter === 0) {
            Qt.quit()
        }
    }
    Component.onCompleted: {
        setTheme(ScreenPlay.settings.theme);
        switchPage("Installed");
        if (!ScreenPlay.settings.silentStart)
            root.show();

    }

    Connections {
        function onThemeChanged(theme) {
            setTheme(theme);
        }

        target: ScreenPlay.settings
    }

    Connections {
        function onRequestNavigation(nav) {
            switchPage(nav);
        }

        target: ScreenPlay.util
    }

    Connections {
        function onRequestRaise() {
            root.show();
        }

        target: ScreenPlay.screenPlayManager
    }

    Dialogs.SteamNotAvailable {
        id: dialogSteam
    }

    Dialogs.MonitorConfiguration {
    }

    Dialogs.CriticalError {
        window: root
    }

    Common.TrayIcon {
        window: root
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

    Connections {
        function onSetSidebarActive(active) {
            if (active)
                sidebar.state = "active";
            else
                sidebar.state = "inactive";
        }

        function onSetNavigationItem(pos) {
            if (pos === 0)
                nav.onPageChanged("Create");
            else
                nav.onPageChanged("Workshop");
        }

        target: stackView.currentItem
        ignoreUnknownSignals: true
    }

    Installed.Sidebar {
        id: sidebar
        objectName: "installedSidebar"

        navHeight: nav.height

        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }

    }

    Navigation.Navigation {
        id: nav
        window: root

        onChangePage: (name)=>  {
            monitors.close();
            switchPage(name);
        }

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

    }

    Monitors.Monitors {
        id: monitors
    }

}
