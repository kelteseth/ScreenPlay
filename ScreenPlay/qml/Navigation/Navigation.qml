import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12
import ScreenPlay 1.0
import "../Workshop"
import "../Common"

Rectangle {
    id: root

    property string currentNavigationName: ""
    property var navArray: [navCreate, navWorkshop, navInstalled, navSettings, navCommunity]
    property bool navActive: true

    signal changePage(string name)

    function setActive(active) {
        navActive = active;
        if (active)
            root.state = "enabled";
        else
            root.state = "disabled";
    }

    function setNavigation(name) {
        var i = 0;
        for (; i < navArray.length; i++) {
            if (navArray[i].name === name) {
                navArray[i].state = "active";
                root.currentNavigationName = name;
            } else {
                navArray[i].state = "inactive";
            }
        }
    }

    function onPageChanged(name) {
        if (!navActive)
            return ;

        root.changePage(name);
        setNavigation(name);
    }

    height: 60
    clip: true
    width: 1366
    color: Material.theme === Material.Light ? "white" : Material.background
    layer.enabled: true

    MouseHoverBlocker {
    }

    Connections {
        function onRequestNavigationActive(isActive) {
            setActive(isActive);
        }

        function onRequestNavigation(nav) {
            onPageChanged(nav);
        }

        target: ScreenPlay.util
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
            onPageClicked: root.onPageChanged(name)
            objectName: "createTab"
        }

        NavigationItem {
            id: navWorkshop

            state: "inactive"
            name: "Workshop"
            iconSource: "qrc:/assets/icons/icon_steam.svg"
            onPageClicked: root.onPageChanged(name)
            objectName: "workshopTab"
        }

        NavigationItem {
            id: navInstalled

            state: "active"
            name: "Installed"
            amount: ScreenPlay.installedListModel.count
            iconSource: "qrc:/assets/icons/icon_installed.svg"
            onPageClicked: root.onPageChanged(name)
            objectName: "installedTab"
        }

        NavigationItem {
            id: navCommunity

            state: "inactive"
            name: "Community"
            iconSource: "qrc:/assets/icons/icon_community.svg"
            onPageClicked: root.onPageChanged(name)
            objectName: "communityTab"
        }

        NavigationItem {
            id: navSettings

            state: "inactive"
            name: "Settings"
            iconSource: "qrc:/assets/icons/icon_settings.svg"
            onPageClicked: root.onPageChanged(name)
            objectName: "settingsTab"
        }

    }

    NavigationWallpaperConfiguration {
    }

    layer.effect: ElevationEffect {
        elevation: 2
    }

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
