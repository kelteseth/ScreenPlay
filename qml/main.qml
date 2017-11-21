import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0
import "Components"

ApplicationWindow {
    id: window
    color: "#eeeeee"
    visible: true
    width: 1400
    minimumHeight: 768
    minimumWidth: 1050

    LinearGradient {
        id: tabShadow
        height: 5
        z:99

        anchors{
            top: nav.bottom
            right: parent.right
            left: parent.left
        }
        start: Qt.point(0, 0)
        end: Qt.point(0,5)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#22000000" }
            GradientStop { position: 1.0; color: "#00000000" }
        }
    }

    SystemTrayIcon {
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
                id:miMuteAll
                property bool isMuted: false
                text: qsTr("Mute all")
                onTriggered: {
                    if(miMuteAll.isMuted){
                        isMuted = false
                        miMuteAll.text = qsTr("Mute all")
                        screenPlaySettings.setMuteAll(false)
                    }else{
                        isMuted = true
                        miMuteAll.text = qsTr("Unmute all")
                        screenPlaySettings.setMuteAll(true)
                    }
                }
            }
            MenuItem {
                id:miStopAll
                property bool isPlaying: false
                text: qsTr("Stop all")
                onTriggered: {
                    if(miStopAll.isPlaying){
                        isPlaying = false
                        miStopAll.text = qsTr("Stop all")
                        screenPlaySettings.setPlayAll(true)
                    }else{
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

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2)
        setY(Screen.height / 2 - height / 2)
    }

    Loader {
        id: pageLoader
        asynchronous: true
        source: "qrc:/qml/Components/Installed.qml"
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        Connections {
            target: pageLoader.item
            ignoreUnknownSignals: true
            onSetSidebaractiveItem: {
                if (sidebar.activeScreen == screenId
                        && sidebar.state == "active") {
                    sidebar.state = "inactive"
                } else {
                    sidebar.state = "active"
                }

                sidebar.activeScreen = screenId
            }
            onSetNavigationItem: {
                if(pos === 0){
                    nav.onPageChanged("Create")
                } else {
                    nav.onPageChanged("Workshop")
                }
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
            pageLoader.setSource("qrc:/qml/Components/" + name + ".qml")
            sidebar.state = "inactive"
        }

        onToggleMonitors: {
            monitors.state = monitors.state == "active" ? "inactive" : "active"
        }
    }

    Monitors {
        id: monitors
        state: "inactive"
        anchors.fill: pageLoader
        z: 98
    }
}
