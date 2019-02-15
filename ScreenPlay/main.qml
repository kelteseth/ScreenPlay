import QtQuick 2.12
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
    title: "ScreenPlay 0.2"
    minimumHeight: 788
    minimumWidth: 1050

    Component.onCompleted: {
        if (!screenPlaySettings.autostart) {
            show()
        }
    }

    function switchPage(name) {
        if (name === "Create") {
            bg.state = "create"
            pageLoader.visible = false
            pageLoaderCreate.visible = true
            pageLoaderWorkshop.visible = false
            pageLoaderCreate.setSource("qrc:/qml/Create/Create.qml")
            sidebar.state = "inactive"
        } else if (name === "Workshop") {
            bg.state = "init"
            pageLoader.visible = false
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = true
            pageLoaderWorkshop.setSource("qrc:/qml/Workshop/Workshop.qml")
            sidebar.state = "inactive"
        } else if (name === "Community") {
            bg.state = "community"
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/Community/Community.qml")
            sidebar.state = "inactive"
        } else {
            bg.state = "init"
            pageLoader.visible = true
            pageLoaderCreate.visible = false
            pageLoaderWorkshop.visible = false
            pageLoader.setSource("qrc:/qml/" + name + "/" + name + ".qml")
            sidebar.state = "inactive"
        }
    }

    Background {
        id: bg
        anchors.fill: parent
    }

    Connections {
        target: screenPlaySettings
        onSetMainWindowVisible: {
            window.visible = visible
            setX(Screen.width / 2 - width / 2)
            setY(Screen.height / 2 - height / 2)
        }
    }

    Connections {
        target: utility
        onRequestNavigation: {
            switchPage(nav)
        }
        onRequestToggleWallpaperConfiguration: {
            monitors.state = monitors.state == "active" ? "inactive" : "active"
            screenPlay.requestProjectSettingsListModelAt(0)
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
                        screenPlay.setAllWallpaperValue("volume", "1")
                    } else {
                        isMuted = true
                        miMuteAll.text = qsTr("Unmute all")
                        screenPlay.setAllWallpaperValue("volume", "0")
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
                        screenPlay.setAllWallpaperValue("isPlaying", "false")
                    } else {
                        isPlaying = true
                        miStopAll.text = qsTr("Play all")
                        screenPlay.setAllWallpaperValue("isPlaying", "true")
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
    }

    Connections {
        target: pageLoader.item
        ignoreUnknownSignals: true

        property bool ignoreWorkshopBanner: false

        onSetSidebarActive: {
            if (active) {
                sidebar.state = "active"
            } else {
                sidebar.state = "inactive"
            }
        }

        onSetSidebaractiveItem: {
            if ((type === "video")) {
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
            } else if (type === "qmlScene") {
                if (sidebar.activeScreen == screenId
                        && sidebar.state == "activeScene") {
                    sidebar.state = "inactive"
                } else {
                    sidebar.state = "activeScene"
                }
            } else if (type === "html") {
                if (sidebar.activeScreen == screenId
                        && sidebar.state == "activeScene") {
                    sidebar.state = "inactive"
                } else {
                    sidebar.state = "activeScene"
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
            if (monitors.state === "active") {
                monitors.state = "inactive"
            }

            switchPage(name)
        }
    }

    Monitors {
        id: monitors
        state: "inactive"
        anchors.fill: pageLoader
        z: 98
    }
}
