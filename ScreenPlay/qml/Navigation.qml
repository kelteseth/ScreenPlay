import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Rectangle {
    id: navigation
    height: 60
    clip: true
    width: 1366
    color: "#ffffff"

    signal changePage(string name)

    property var navArray: [navCreate, navWorkshop, navInstalled, navSettings, navCommunity, navScreen]
    property bool navActive: true

    Connections {
        target: utility
        onRequestNavigationActive: {
            setActive(isActive)
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

        //Avoid crashing screenplay when steam is not available
        if (name === "Workshop" && screenPlaySettings.offlineMode) {
            steamWorkshop.initSteam()
            return
        }

        if (!navActive)
            return

        navigation.changePage(name)

        for (var i = 0; i < navArray.length; i++) {
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
            iconSource: "qrc:/assets/icons/icon_steam.svg"
            onPageClicked: navigation.onPageChanged(name)
        }

        NavigationItem {
            id: navInstalled
            state: "active"
            name: "Installed"
            iconSource: "qrc:/assets/icons/icon_installed.svg"
            onPageClicked: navigation.onPageChanged(name)
        }

        NavigationItem {
            id: navScreen
            state: "inactive"
            name: "Screen"
            iconSource: "qrc:/assets/icons/icon_screen.svg"
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

    NavigationWallpaperConfiguration {
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
