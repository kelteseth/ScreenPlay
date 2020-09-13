import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

import ScreenPlay 1.0
import Settings 1.0

import ScreenPlay.Shader 1.0
import "ShaderWrapper" as ShaderWrapper

import "qml/"
import "qml/Monitors" as Monitors
import "qml/Common" as Common
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
    title: "ScreenPlay Alpha - V0.12.0"
    minimumHeight: 450
    minimumWidth: 1050
    onVisibilityChanged: {
        if (window.visibility === 2) {
            switchPage("Installed")
        }
    }



    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)

    Component.onCompleted: {
        setTheme(ScreenPlay.settings.theme)

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
        if (name === "Create") {
            bg.state = "create"
            pageLoader.visible = false
            pageLoaderCreate.visible = true
            pageLoaderWorkshop.visible = false
            pageLoaderCreate.setSource("qrc:/qml/Create/Create.qml")
            pageLoaderCreate.item.checkFFMPEG()
        } else if (name === "Workshop") {
            if (ScreenPlay.settings.steamVersion) {

                bg.state = "init"
                pageLoader.visible = false
                pageLoaderCreate.visible = false
                pageLoaderWorkshop.visible = true
                pageLoaderWorkshop.setSource("qrc:/qml/Workshop/Workshop.qml")
            }
        } else if (name === "Community") {
            bg.state = "community"
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/Community/Community.qml")
        } else {
            bg.state = "init"
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/" + name + "/" + name + ".qml")
        }

        sidebar.state = "inactive"
    }

    Common.TrayIcon {}

    Common.Background {
        id: bg
        anchors.fill: parent
    }


    Loader {
        id: pageLoader
        asynchronous: true
        source: "qrc:/qml/Installed/Installed.qml"
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        onStatusChanged: {
            if (status == Loader.Ready) {
                if (pageLoaderCreate.source != "qrc:/qml/Create/Create.qml")
                    timerLoader.start()
            }
        }
    }

    Timer {
        id: timerLoader
        interval: 500
        onTriggered: {
            pageLoaderCreate.source = "qrc:/qml/Create/Create.qml"
        }
    }

    Loader {
        id: pageLoaderCreate
        visible: false
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
    }

    Loader {
        id: pageLoaderWorkshop
        visible: false
        enabled: ScreenPlay.settings.steamVersion
        asynchronous: true
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
    }

    Connections {
        target: pageLoader.item
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
