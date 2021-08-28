import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import ScreenPlay 1.0
import Settings 1.0
import "qml/Monitors" as Monitors
import "qml/Common" as Common
import "qml/Common/Dialogs" as Dialogs
import "qml/Installed" as Installed
import "qml/Navigation" as Navigation
import "qml/Workshop" as Workshop
import "qml/Community" as Community

ApplicationWindow {
    id: window

    function setTheme(theme) {
        switch (theme) {
        case Settings.System:
            window.Material.theme = Material.System;
            break;
        case Settings.Dark:
            window.Material.theme = Material.Dark;
            break;
        case Settings.Light:
            window.Material.theme = Material.Light;
            break;
        }
    }

    function switchPage(name) {
        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                ScreenPlay.installedListModel.reset();

        }
        stackView.replace("qrc:/qml/" + name + "/" + name + ".qml");
        sidebar.state = "inactive";
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
        if (window.visibility === 2)
            ScreenPlay.installedListModel.reset();

    }
    Component.onCompleted: {
        setTheme(ScreenPlay.settings.theme);
        switchPage("Installed");
        if (!ScreenPlay.settings.silentStart)
            window.show();

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
            window.show();
        }

        target: ScreenPlay.screenPlayManager
    }

    Dialogs.SteamNotAvailable {
        id: dialogSteam
    }

    Dialogs.MonitorConfiguration {
    }

    Dialogs.CriticalError {
        mainWindow: window
    }

    Common.TrayIcon {
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

        onChangePage: {
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
