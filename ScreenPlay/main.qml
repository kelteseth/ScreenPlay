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
    color: Material.theme === Material.Dark ? Qt.darker(
                                                  Material.background) : Material.background
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 788
    title: "ScreenPlay Alpha - 0.13.0"
    minimumHeight: 450
    minimumWidth: 1050
    onVisibilityChanged: {
        if (window.visibility === 2) {
            ScreenPlay.installedListModel.reset()
        }
    }

    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)

    Component.onCompleted: {
        setTheme(ScreenPlay.settings.theme)
        switchPage("Installed")

        if (!ScreenPlay.settings.silentStart) {
            window.show()
            ScreenPlay.setTrackerSendEvent("navigation", "Installed")
        } else {
            ScreenPlay.setTrackerSendEvent("navigation", "Silent")
        }
    }

    Connections {
        target: ScreenPlay.settings
        function onThemeChanged(theme) {
            setTheme(theme)
        }
    }

    Connections {
        target: ScreenPlay.util
        function onRequestNavigation(nav) {
            switchPage(nav)
        }
    }

    Connections {
        target: ScreenPlay.screenPlayManager
        function onRequestRaise() {
            window.show()
        }
    }

    function setTheme(theme) {
        switch (theme) {
        case Settings.System:
            window.Material.theme = Material.System
            break
        case Settings.Dark:
            window.Material.theme = Material.Dark
            break
        case Settings.Light:
            window.Material.theme = Material.Light
            break
        }
    }

    function switchPage(name) {
        const unloadSteamPlugin = nav.currentNavigationName === "Workshop"

        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                ScreenPlay.installedListModel.reset()
            return
        }

        if (name === "Workshop") {
            if (!ScreenPlay.settings.steamVersion) {
                const steamAvialable = ScreenPlay.loadSteamPlugin()
                if (!steamAvialable) {
                    dialogSteam.open()
                    switchPage("Installed")
                    return
                }
            }
        }

        stackView.replace("qrc:/qml/" + name + "/" + name + ".qml")

        if (unloadSteamPlugin) {
            ScreenPlay.unloadSteamPlugin()
        }

        sidebar.state = "inactive"
    }

    Dialogs.SteamNotAvailable {
        id: dialogSteam
    }

    Dialogs.MonitorConfiguration {}

    Dialogs.CriticalError {
        mainWindow: window
    }

    Common.TrayIcon {}

    StackView {
        id: stackView
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        property int duration: 300

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
        target: stackView.currentItem
        ignoreUnknownSignals: true

        function onSetSidebarActive(active) {
            if (active) {
                sidebar.state = "active"
            } else {
                sidebar.state = "inactive"
            }
        }

        function onSetNavigationItem(pos) {
            if (pos === 0) {
                nav.onPageChanged("Create")
            } else {
                nav.onPageChanged("Workshop")
            }
        }
    }

    Installed.Sidebar {
        id: sidebar

        navHeight: nav.height
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
    }

    Navigation.Navigation {
        id: nav
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        onChangePage: {
            monitors.close()
            switchPage(name)
        }
    }

    Monitors.Monitors {
        id: monitors
    }
}
