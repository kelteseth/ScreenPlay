import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import Settings
import ScreenPlayUtil as Util
import Qt5Compat.GraphicalEffects
import Qt.labs.settings 1.0 as Labs
import "qml/Monitors" as Monitors
import "qml/Installed" as Installed
import "qml/Navigation" as Navigation
import "qml/Community" as Community

ApplicationWindow {
    id: root

    function setTheme(theme) {
        switch (theme) {
        case Settings.System:
            root.Material.theme = Material.System
            break
        case Settings.Dark:
            root.Material.theme = Material.Dark
            break
        case Settings.Light:
            root.Material.theme = Material.Light
            break
        }
    }

    function switchPage(name) {
        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                App.installedListModel.reset()
        }

        if (name === "Installed") {
            stackView.replace(
                        "qrc:/qml/ScreenPlayApp/qml/Installed/Installed.qml", {
                            "sidebar": sidebar
                        })
            return
        }
        stackView.replace(
                    "qrc:/qml/ScreenPlayApp/qml/" + name + "/" + name + ".qml",
                    {
                        "modalSource": content
                    })
        sidebar.state = "inactive"
    }

    color: Material.theme === Material.Dark ? Qt.darker(
                                                  Material.background) : Material.background
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 810
    title: "ScreenPlay Alpha - V" + App.version()
    minimumHeight: 450
    minimumWidth: 1050

    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)
    onVisibilityChanged: {
        if (root.visibility === 2)
            App.installedListModel.reset()
    }
    onClosing: (close) => {
        close.accepted = false

        if (App.screenPlayManager.activeWallpaperCounter === 0
                && App.screenPlayManager.activeWidgetsCounter === 0) {
            Qt.quit()
        }

        const alwaysMinimize = settings.value("alwaysMinimize", null)
        if(alwaysMinimize === null){
            console.error("Unable to retreive alwaysMinimize setting")
        }

        if(alwaysMinimize === "true"){
           root.hide()
           App.showDockIcon(false);
           return
        }

        exitDialog.open()
    }

    Labs.Settings {
        id: settings
    }

    Navigation.ExitPopup {
        id: exitDialog
        applicationWindow: root
        modalSource: content
    }

    Component.onCompleted: {
        setTheme(App.settings.theme)
        stackView.push("qrc:/qml/ScreenPlayApp/qml/Installed/Installed.qml", {
                           "sidebar": sidebar
                       })
        if (!App.settings.silentStart){
            App.showDockIcon(true)
            root.show()
        }
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

        Monitors.Monitors {
            id: monitors
            modalSource: content
        }
        Util.TrayIcon {
            window: root
        }
    }

    Connections {
        function onThemeChanged(theme) {
            setTheme(theme)
        }

        target: App.settings
    }

    Connections {
        function onRequestNavigation(nav) {
            switchPage(nav)
        }

        target: App.util
    }

    Connections {
        function onRequestRaise() {
            App.showDockIcon(true)
            root.show()
        }

        target: App.screenPlayManager
    }

    Item {
        id: content
        anchors.fill:parent


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
                sidebar.state = "active"
            else
                sidebar.state = "inactive"
        }

        function onSetNavigationItem(pos) {
            if (pos === 0)
                nav.onPageChanged("Create")
            else
                nav.onPageChanged("Workshop")
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
            monitors.close()
            switchPage(name)
        }
    }
}
}
