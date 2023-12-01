import QtCore as QCore
import QtQml
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import ScreenPlayUtil as Util
import Qt5Compat.GraphicalEffects
import Plausible 1.0
import "qml/Monitors" as Monitors
import "qml/Installed" as Installed
import "qml/Navigation" as Navigation
import "qml/Community" as Community
import "qml"

ApplicationWindow {
    id: root

    function setTheme(theme) {
        switch (theme) {
        case Settings.Theme.System:
            root.Material.theme = Material.System;
            break;
        case Settings.Theme.Dark:
            root.Material.theme = Material.Dark;
            break;
        case Settings.Theme.Light:
            root.Material.theme = Material.Light;
            break;
        }
    }

    function switchPage(name) {
        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                App.installedListModel.reset();
        }
        if (name === "Installed") {
            stackView.replace("qrc:/qml/ScreenPlayApp/qml/Installed/InstalledView.qml", {
                    "sidebar": sidebar
                });
            return;
        }
        stackView.replace("qrc:/qml/ScreenPlayApp/qml/" + name + "/" + name + "View.qml", {
                "modalSource": content
            });
        nav.setNavigation(name);
        sidebar.state = "inactive";
    }

    color: Material.theme === Material.Dark ? Qt.darker(Material.background) : Material.background
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 810
    title: "ScreenPlay Alpha - v" + App.version()
    minimumHeight: 450
    minimumWidth: 1050

    Plausible {
        id: plausible
        screenSize: Qt.size(root.width, root.height)
        domain: "app.screen-play.app"
        debug: false
        enabled: false
    }

    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)
    onVisibilityChanged: {
        if (root.visibility !== 2)
            return;
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
        exitDialog.open();
    }

    QCore.Settings {
        id: settings
    }

    Navigation.ExitPopup {
        id: exitDialog
        applicationWindow: root
        modalSource: content
    }

    Component.onCompleted: {
         print("Settings.Language.Pl_PL",Settings.Language.Pl_PL)
        print(App.settings.theme,Settings.Theme.Light);
        setTheme(App.settings.theme);
        stackView.push("qrc:/qml/ScreenPlayApp/qml/Installed/InstalledView.qml", {
                "sidebar": sidebar
            });
        if (!App.settings.silentStart) {
            App.showDockIcon(true);
            root.show();
        }
        App.installedListModel.reset();
    }

    Item {
        id: noneContentItems
        Util.SteamNotAvailable {
            id: dialogSteam
            modalSource: content
        }

        Util.MonitorConfiguration {
            modalSource: content
        }

        Util.CriticalError {
            window: root
            modalSource: content
        }

        Monitors.MonitorsView {
            id: monitors
            modalSource: content
        }
        TrayIcon {
            window: root
        }
    }

    Connections {
        function onThemeChanged(theme) {
            setTheme(theme);
        }

        target: App.settings
    }

    Connections {
        function onRequestNavigation(nav) {
            switchPage(nav);
        }

        target: App.util
    }

    Connections {
        function onRequestRaise() {
            App.showDockIcon(true);
            root.show();
        }

        function onActiveWidgetsCounterChanged() {
            plausible.pageView("widget/count/" + App.screenPlayManager.activeWidgetsCounter);
        }

        function onActiveWallpaperCounterChanged() {
            plausible.pageView("wallpaper/count/" + App.screenPlayManager.activeWallpaperCounter);
        }

        target: App.screenPlayManager
    }

    Item {
        id: content
        anchors.fill: parent

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
            modalSource: content
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            onChangePage: function (name) {
                monitors.close();
                switchPage(name);
            }
        }
    }
}
