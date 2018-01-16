import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

import "qml/"
import "qml/Installed"

ApplicationWindow {
    id: window
    color: "#eeeeee"
    // Set visible if the -silent parameter was not set
    visible: false
    width: 1400
    minimumHeight: 768
    minimumWidth: 1050

    Connections {
        target: screenPlaySettings
        onSetMainWindowVisible: {
            window.visible = visible
            setX(Screen.width / 2 - width / 2)
            setY(Screen.height / 2 - height / 2)
        }
    }

    LinearGradient {
        id: tabShadow
        height: 6
        z: 500
        cached: true

        anchors {
            top: nav.bottom
            right: parent.right
            left: parent.left
        }
        start: Qt.point(0, 0)
        end: Qt.point(0, 6)
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
        iconSource: "qrc:/assets/icons/favicon.ico"

        menu: Menu {
            MenuItem {
                text: qsTr("Open ScreenPlay")
                onTriggered: {
                    window.show()
                }
            }
            MenuItem {
                id: miMuteAll
                property bool isMuted: false
                text: qsTr("Mute all")
                onTriggered: {
                    if (miMuteAll.isMuted) {
                        isMuted = false
                        miMuteAll.text = qsTr("Mute all")
                        screenPlaySettings.setMuteAll(false)
                    } else {
                        isMuted = true
                        miMuteAll.text = qsTr("Unmute all")
                        screenPlaySettings.setMuteAll(true)
                    }
                }
            }
            MenuItem {
                id: miStopAll
                property bool isPlaying: false
                text: qsTr("Stop all")
                onTriggered: {
                    if (miStopAll.isPlaying) {
                        isPlaying = false
                        miStopAll.text = qsTr("Pause all")
                        screenPlaySettings.setPlayAll(true)
                    } else {
                        isPlaying = true
                        miStopAll.text = qsTr("Play all")
                        screenPlaySettings.setPlayAll(false)
                    }
                }
            }
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
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
        asynchronous: true
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
        asynchronous: true
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        onStatusChanged: {
            if (status == Loader.Ready) {
                connectionsPageLoaderWorkshop.target = pageLoaderWorkshop
            }
        }
    }
    Connections {
        id: connectionsPageLoaderWorkshop
        ignoreUnknownSignals: true
        onOpenCreate: {
            print("lellele")
            if (!ignoreWorkshopBanner) {
                switchPage("Workshop")
                ignoreWorkshopBanner = true
            }
        }
    }
    Connections {
        target: pageLoader.item
        ignoreUnknownSignals: true

        property bool ignoreWorkshopBanner: false

        onSetSidebaractiveItem: {

            if (type === "video") {
                if (sidebar.activeScreen == screenId
                        && sidebar.state == "active") {
                    sidebar.state = "inactive"
                } else {
                    sidebar.state = "active"
                }
            } else if (type === "widget") {
                if (sidebar.activeScreen == screenId
                        && sidebar.state == "activeWidget") {
                    sidebar.state = "inactive"
                } else {
                    sidebar.state = "activeWidget"
                }
            }
            sidebar.activeScreen = screenId
            sidebar.type = type
        }
        onSetNavigationItem: {
            if (pos === 0) {
                nav.onPageChanged("Create")
            } else {
                nav.onPageChanged("Workshop")
            }
        }
    }

    Sidebar {
        id: sidebar
        width: 400
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
            switchPage(name)
        }

        onToggleMonitors: {
            monitors.state = monitors.state == "active" ? "inactive" : "active"
        }
    }

    function switchPage(name) {
        if (name === "Create") {
            pageLoader.visible = false
            pageLoaderCreate.setSource("qrc:/qml/Create/Create.qml")
            pageLoaderCreate.visible = true
            pageLoaderWorkshop.visible = false
            sidebar.state = "inactive"
        } else if (name === "Workshop") {
            pageLoader.visible = false
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.setSource("qrc:/qml/Workshop/Workshop.qml")
            pageLoaderWorkshop.visible = true
            sidebar.state = "inactive"
        } else {
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/" + name + "/" + name + ".qml")
            sidebar.state = "inactive"
        }
    }

    Monitors {
        id: monitors
        state: "inactive"
        anchors.fill: pageLoader
        z: 98
    }
}