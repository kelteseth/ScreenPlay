import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material.impl 2.12
import ScreenPlay 1.0

import "../Workshop"
import "../Common"

Rectangle {
    id: navigation
    height: 60
    clip: true
    width: 1366
    color: Material.theme === Material.Light ? "white" : Material.background
    layer.enabled: true
    layer.effect: ElevationEffect {
        elevation: 2
    }

    MouseHoverBlocker {}

    signal changePage(string name)

    property var navArray: [navCreate, navWorkshop, navInstalled, navSettings, navCommunity]
    property bool navActive: true

    Connections {
        target: ScreenPlay.util
        function onRequestNavigationActive(isActive) {
            setActive(isActive)
        }
        function onRequestNavigation(nav) {
            onPageChanged(nav)
        }
    }

    function setActive(active) {
        navActive = active
        if (active) {
            navigation.state = "enabled"
        } else {
            navigation.state = "disabled"
        }
    }

    function onPageChanged(name) {

        ScreenPlay.setTrackerSendEvent("navigation", name)

        if (!navActive)
            return

        navigation.changePage(name)

        var i = 0
        for (; i < navArray.length; i++) {
            if (navArray[i].name === name)
                navArray[i].state = "active"
            else {
                navArray[i].state = "inactive"
            }
        }
    }

    Row {
        id: row
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: 20
        spacing: 0

        NavigationItem {
            id: navCreate
            state: "inactive"
            name: "Create"

            iconSource: "qrc:/assets/icons/icon_plus.svg"
            onPageClicked: navigation.onPageChanged(name)
        }

        NavigationItem {
            id: navWorkshop
            state: "inactive"
            name: "Workshop"
            enabled: ScreenPlay.settings.steamVersion
            iconSource: "qrc:/assets/icons/icon_steam.svg"
            onPageClicked: navigation.onPageChanged(name)
        }

        NavigationItem {
            id: navInstalled
            state: "active"
            name: "Installed"
            amount: ScreenPlay.installedListModel.count
            iconSource: "qrc:/assets/icons/icon_installed.svg"
            onPageClicked: navigation.onPageChanged(name)
        }

        NavigationItem {
            id: navCommunity
            state: "inactive"
            name: "Community"
            iconSource: "qrc:/assets/icons/icon_community.svg"
            onPageClicked: navigation.onPageChanged(name)
        }
        NavigationItem {
            id: navSettings
            state: "inactive"
            name: "Settings"
            iconSource: "qrc:/assets/icons/icon_settings.svg"
            onPageClicked: navigation.onPageChanged(name)
        }
    }

    NavigationWallpaperConfiguration {}
    states: [
        State {
            name: "enabled"
        },
        State {
            name: "disabled"

            PropertyChanges {
                target: row
                opacity: 0.3
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"
            PropertyAnimation {
                target: row
                duration: 300
            }
        }
    ]
}
