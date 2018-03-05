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
    signal toggleMonitors

    Action {
        shortcut: "F1"
        onTriggered: onPageChanged("Create")
    }
    Action {
        shortcut: "F2"
        onTriggered: onPageChanged("Workshop")
    }
    Action {
        shortcut: "F3"
        onTriggered: onPageChanged("Installed")
    }
    Action {
        shortcut: "F4"
        onTriggered: onPageChanged("Community")
    }
    Action {
        shortcut: "F5"
        onTriggered: onPageChanged("Settings")
    }

    function onPageChanged(name) {

        //Avoid crashing screenplay when steam is not available
        if(name === "Workshop" && screenPlaySettings.offlineMode){
            return
        }

        navigation.changePage(name)
        if (name === "Workshop") {
            navWorkshop.state = "active"
            navInstalled.state = "inactive"
            navSettings.state = "inactive"
            navCommunity.state = "inactive"
            navCreate.state = "inactive"
        } else if (name === "Installed") {
            navWorkshop.state = "inactive"
            navInstalled.state = "active"
            navSettings.state = "inactive"
            navCommunity.state = "inactive"
            navCreate.state = "inactive"
        } else if (name === "Settings") {
            navWorkshop.state = "inactive"
            navInstalled.state = "inactive"
            navSettings.state = "active"
            navCommunity.state = "inactive"
            navCreate.state = "inactive"
        } else if (name === "Create") {
            navWorkshop.state = "inactive"
            navInstalled.state = "inactive"
            navSettings.state = "inactive"
            navCreate.state = "active"
            navCommunity.state = "inactive"
        } else if (name === "Community") {
            navWorkshop.state = "inactive"
            navInstalled.state = "inactive"
            navSettings.state = "inactive"
            navCommunity.state = "active"
            navCreate.state = "inactive"
        }
    }

    Row {
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

    Item {
        id: monitorSelection
        width: 321
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                toggleMonitors()
            }
        }

        Image {
            id: image
            width: 24
            height: 24
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            sourceSize.height: 24
            sourceSize.width: 24
            source: "qrc:/assets/icons/icon_monitor.svg"

            Text {
                id: txtAmountActiveWallpapers
                text: screenPlaySettings.activeWallpaperCounter
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    topMargin: 1
                }

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "orange"
                font.pointSize: 10
                font.bold: true
                font.family: "Roboto"
                renderType: Text.NativeRendering
            }
        }

        Text {
            id: activeMonitorName
            text: qsTr("Multi Monitor Setup")
            anchors.right: image.right
            anchors.rightMargin: 10 + image.width
            horizontalAlignment: Text.AlignRight
            color: "#626262"
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 12
            font.family: "Roboto"
            renderType: Text.NativeRendering
        }
    }
}
