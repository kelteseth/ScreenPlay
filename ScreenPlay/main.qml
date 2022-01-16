import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay 1.0
import Settings 1.0
import Qt5Compat.GraphicalEffects
import "qml/Monitors" as Monitors
import "qml/Common" as Common
import "qml/Common/Dialogs" as Dialogs
import "qml/Installed" as Installed
import "qml/Navigation" as Navigation
import "qml/Workshop" as Workshop
import "qml/Community" as Community

ApplicationWindow {
    id: root

    function setTheme(theme) {
        switch (theme) {
        case Settings.System:
            root.Material.theme = Material.System
            break
        case Settings.Dark:
            root.Material.theme = Material.Dark
            break
        case Settings.Light:
            root.Material.theme = Material.Light
            break
        }
    }

    function switchPage(name) {
        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                ScreenPlay.installedListModel.reset()
        }

        if (name === "Installed") {
            stackView.replace("qrc:/ScreenPlay/qml/Installed/Installed.qml", {
                                  "sidebar": sidebar
                              })
            return
        }
        stackView.replace("qrc:/ScreenPlay/qml/" + name + "/" + name + ".qml", {
                              "modalSource": content
                          })
        sidebar.state = "inactive"
    }

    color: Material.theme === Material.Dark ? Qt.darker(
                                                  Material.background) : Material.background
    // Set visible if the -silent parameter was not set (see app.cpp end of constructor).
    visible: false
    width: 1400
    height: 788
    title: "ScreenPlay Alpha - " + ScreenPlay.version()
    minimumHeight: 450
    minimumWidth: 1050
    flags: Qt.FramelessWindowHint | Qt.Window

    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)
    onVisibilityChanged: {
        if (root.visibility === 2)
            ScreenPlay.installedListModel.reset()
    }
    onClosing: {
        if (ScreenPlay.screenPlayManager.activeWallpaperCounter === 0
                && ScreenPlay.screenPlayManager.activeWidgetsCounter === 0) {
            Qt.quit()
        }
    }
    Component.onCompleted: {
        setTheme(ScreenPlay.settings.theme)
        stackView.push("qrc:/ScreenPlay/qml/Installed/Installed.qml", {
                           "sidebar": sidebar
                       })
        if (!ScreenPlay.settings.silentStart)
            root.show()
    }

    Item {
        id: content
        anchors.fill: parent
        anchors.margins: 1

        Connections {
            function onThemeChanged(theme) {
                setTheme(theme)
            }

            target: ScreenPlay.settings
        }

        Connections {
            function onRequestNavigation(nav) {
                switchPage(nav)
            }

            target: ScreenPlay.util
        }

        Connections {
            function onRequestRaise() {
                root.show()
            }

            target: ScreenPlay.screenPlayManager
        }

        Dialogs.SteamNotAvailable {
            id: dialogSteam
            modalSource: content
        }

        Dialogs.MonitorConfiguration {
            modalSource: content
        }

        Dialogs.CriticalError {
            window: root
            modalSource: content
        }

        Common.TrayIcon {
            window: root
        }

        StackView {
            id: stackView
            objectName: "stackView"
            property int duration: 300

            anchors {
                top: nav.bottom
                right: parent.right
                bottom: parent.bottom
                left: parent.left
            }

            replaceEnter: Transition {
                OpacityAnimator {
                    from: 0
                    to: 1
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }

                ScaleAnimator {
                    from: 0.8
                    to: 1
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }
            }

            replaceExit: Transition {
                OpacityAnimator {
                    from: 1
                    to: 0
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }

                ScaleAnimator {
                    from: 1
                    to: 0.8
                    duration: stackView.duration
                    easing.type: Easing.InOutQuart
                }
            }
        }

        Connections {
            function onSetSidebarActive(active) {
                if (active)
                    sidebar.state = "active"
                else
                    sidebar.state = "inactive"
            }

            function onSetNavigationItem(pos) {
                if (pos === 0)
                    nav.onPageChanged("Create")
                else
                    nav.onPageChanged("Workshop")
            }

            target: stackView.currentItem
            ignoreUnknownSignals: true
        }

        Installed.Sidebar {
            id: sidebar
            objectName: "installedSidebar"
            navHeight: nav.height

            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }
        }

        Navigation.Navigation {
            id: nav
            window: root
            width: parent.width
            modalSource: content
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            onChangePage: function (name) {
                monitors.close()
                switchPage(name)
            }
        }

        Monitors.Monitors {
            id: monitors
            modalSource: content
        }
    }

    Rectangle {
        height: 1
        color: "#222"
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }
    Rectangle {
        height: 1
        color: "#222"
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: parent.left
        }
    }
    Rectangle {
        width: 1
        color: "#222"
        anchors {
            left: parent.left
            bottom: parent.bottom
            top: parent.top
        }
    }
    Rectangle {
        width: 1
        color: "#222"
        anchors {
            right: parent.right
            bottom: parent.bottom
            top: parent.top
        }
    }

    Rectangle {
        width: 15
        height: width
        color: "#555"
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 1
        }
        MouseArea {
            id: maResize
            anchors.fill: parent
            cursorShape: Qt.SizeFDiagCursor
            property point clickPosition
            property size originalSize
            onPressed: {
                maResize.clickPosition = Qt.point(maResize.mouseX,
                                                  maResize.mouseY)
                maResize.clickPosition = maResize.mapToGlobal(
                            maResize.clickPosition.x, maResize.clickPosition.y)
                maResize.originalSize = Qt.size(root.width, root.height)
            }
            onPositionChanged: {
                if (maResize.pressed) {
                    let newPos = maResize.mapToGlobal(maResize.mouseX,
                                                      maResize.mouseY)
                    let newPosX = newPos.x - maResize.clickPosition.x
                    let newPosY = newPos.y - maResize.clickPosition.y

                    root.setGeometry(root.x, root.y,
                                     maResize.originalSize.width + newPosX,
                                     maResize.originalSize.height + newPosY)
                }
            }
        }
    }
}
