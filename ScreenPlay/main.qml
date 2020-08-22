import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

import ScreenPlay 1.0
import Settings 1.0

import "qml/"
import "qml/Monitors"
import "qml/Common"
import "qml/Installed"
import "qml/Navigation"
import "qml/Workshop"
import "qml/Community"

ApplicationWindow {
    id: window
    color: {
        if (Material.theme === Material.Dark) {
            return Qt.darker(Material.background)
        } else {
            return Material.background
        }
    }

    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 788
    title: "ScreenPlay Alpha - V0.11.0"
    minimumHeight: 450
    minimumWidth: 1050
    onVisibilityChanged: {
        if (window.visibility === 2) {
            switchPage("Installed")
        }
    }

    Material.accent: {
        return Material.color(Material.Orange)
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
    Connections {
        target: ScreenPlay.settings
        function onThemeChanged(theme) {
            setTheme(theme)
        }
    }

    Component.onCompleted: {
        setTheme(ScreenPlay.settings.theme)

        if (!ScreenPlay.settings.silentStart) {
            window.show()
            ScreenPlay.setTrackerSendEvent("navigation", "Installed")
        } else {
            ScreenPlay.setTrackerSendEvent("navigation", "Silent")
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

    Background {
        id: bg
        anchors.fill: parent
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

    LinearGradient {
        id: tabShadow
        height: 4
        visible: true
        z: 500
        cached: true

        anchors {
            top: nav.bottom
            right: parent.right
            left: parent.left
        }
        start: Qt.point(0, 0)
        end: Qt.point(0, 4)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#33000000"
            }
            GradientStop {
                position: 1.0
                color: "#00000000"
            }
        }
    }

    SystemTrayIcon {
        id: sti
        visible: true
        iconSource: "qrc:/assets/icons/app.ico"
        tooltip: qsTr("ScreenPlay - Double click to change you settings.")
        onActivated: {
            switch (reason) {
            case SystemTrayIcon.Unknown:
                break
            case SystemTrayIcon.Context:
                break
            case SystemTrayIcon.DoubleClick:
                window.show()
                break
            case SystemTrayIcon.Trigger:
                break
            case SystemTrayIcon.MiddleClick:
                break
            }
        }

        menu: Menu {
            MenuItem {
                text: qsTr("Open ScreenPlay")
                onTriggered: {
                    window.show()
                }
            }
            MenuItem {
                id: miMuteAll
                property bool isMuted: true
                text: qsTr("Mute all")
                onTriggered: {
                    if (miMuteAll.isMuted) {
                        isMuted = false
                        miMuteAll.text = qsTr("Mute all")
                        ScreenPlay.screenPlayManager.setAllWallpaperValue(
                                    "muted", "true")
                    } else {
                        isMuted = true
                        miMuteAll.text = qsTr("Unmute all")
                        ScreenPlay.screenPlayManager.setAllWallpaperValue(
                                    "muted", "false")
                    }
                }
            }
            MenuItem {
                id: miStopAll
                property bool isPlaying: false
                text: qsTr("Pause all")
                onTriggered: {
                    if (miStopAll.isPlaying) {
                        isPlaying = false
                        miStopAll.text = qsTr("Pause all")
                        ScreenPlay.screenPlayManager.setAllWallpaperValue(
                                    "isPlaying", "true")
                    } else {
                        isPlaying = true
                        miStopAll.text = qsTr("Play all")
                        ScreenPlay.screenPlayManager.setAllWallpaperValue(
                                    "isPlaying", "false")
                    }
                }
            }
            MenuItem {
                text: qsTr("Quit")
                onTriggered: ScreenPlay.exit()
            }
        }
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

    Sidebar {
        id: sidebar

        navHeight: nav.height
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
    }

    Navigation {
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

    Monitors {
        id: monitors
    }
}
