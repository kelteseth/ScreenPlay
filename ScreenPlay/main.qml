import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import ScreenPlay 1.0
import Settings 1.0

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
    title: "ScreenPlay Alpha - V0.13.0"
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
            pageLoader.visible = false
            pageLoaderCreate.visible = true
            pageLoaderWorkshop.visible = false
            pageLoaderCreate.setSource("qrc:/qml/Create/Create.qml")
        } else if (name === "Workshop") {
            if (!ScreenPlay.settings.steamVersion) {
                const steamAvialable = ScreenPlay.loadSteamPlugin()
                if (!steamAvialable) {
                    dialogSteam.open()
                    switchPage("Installed")
                    return
                }
            }
            pageLoader.visible = false
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = true
            pageLoaderWorkshop.setSource("qrc:/qml/Workshop/Workshop.qml")
        } else if (name === "Community") {
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/Community/Community.qml")
        } else {
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/" + name + "/" + name + ".qml")
        }

        sidebar.state = "inactive"
    }

    Dialog {
        id: dialogSteam
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok
        title: qsTr("Could not load steam integration!")
    }

    Dialog {
        id: dialogMonitorConfigurationChanged
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok
        contentHeight: 250

        contentItem: Item {
            ColumnLayout {

                anchors.margins: 20
                anchors.fill: parent
                spacing: 20

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 150
                    source: "qrc:/assets/icons/monitor_setup.svg"
                    fillMode: Image.PreserveAspectFit
                }

                Text {
                    text: qsTr("Your monitor setup changed!\n Please configure your wallpaper again.")
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    horizontalAlignment: Text.AlignHCenter
                    font.family: ScreenPlay.settings.font
                    font.pointSize: 16
                    color: Material.primaryTextColor
                }
            }
        }
        Connections {
            target: ScreenPlay.monitorListModel
            function onMonitorConfigurationChanged() {
                dialogMonitorConfigurationChanged.open()
            }
        }
    }

    Common.TrayIcon {}


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
