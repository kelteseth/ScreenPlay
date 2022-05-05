import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import Settings
import ScreenPlayUtil
import Qt5Compat.GraphicalEffects
import "qml/Monitors" as Monitors
import "qml/Installed" as Installed
import "qml/Navigation" as Navigation
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
                App.installedListModel.reset()
        }

        if (name === "Installed") {
            stackView.replace(
                        "qrc:/qml/ScreenPlayApp/qml/Installed/Installed.qml", {
                            "sidebar": sidebar
                        })
            return
        }
        stackView.replace(
                    "qrc:/qml/ScreenPlayApp/qml/" + name + "/" + name + ".qml",
                    {
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
    title: "ScreenPlay Alpha - V" + App.version()
    minimumHeight: 450
    minimumWidth: 1050
    property bool enableCustomWindowNavigation: Qt.platform.os === "windows"
                                                || Qt.platform.os === "osx"

    // Partial workaround for
    // https://bugreports.qt.io/browse/QTBUG-86047
    Material.accent: Material.color(Material.Orange)
    onVisibilityChanged: {
        if (root.visibility === 2)
            App.installedListModel.reset()
    }
    onClosing: {
        if (App.screenPlayManager.activeWallpaperCounter === 0
                && App.screenPlayManager.activeWidgetsCounter === 0) {
            Qt.quit()
        }
    }
    Component.onCompleted: {
        if (root.enableCustomWindowNavigation) {
            root.flags = Qt.FramelessWindowHint | Qt.Window
        }
        setTheme(App.settings.theme)
        stackView.push("qrc:/qml/ScreenPlayApp/qml/Installed/Installed.qml", {
                           "sidebar": sidebar
                       })
        if (!App.settings.silentStart)
            root.show()
    }

    Item {
        id: noneContentItems
        SteamNotAvailable {
            id: dialogSteam
            modalSource: content
        }

        MonitorConfiguration {
            modalSource: content
        }

        CriticalError {
            window: root
            modalSource: content
        }

        Monitors.Monitors {
            id: monitors
            modalSource: content
        }
        TrayIcon {
            window: root
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: 1
        Navigation.WindowNavigation {
            id: windowNav
            enabled: root.enableCustomWindowNavigation
            visible: enabled
            z: 5
            modalSource: content
            width: parent.width
            window: root
        }

        Item {
            id: content
            anchors {
                top: root.enableCustomWindowNavigation ? windowNav.bottom : parent.top
                right: parent.right
                bottom: parent.bottom
                left: parent.left
            }

            Connections {
                function onThemeChanged(theme) {
                    setTheme(theme)
                }

                target: App.settings
            }

            Connections {
                function onRequestNavigation(nav) {
                    switchPage(nav)
                }

                target: App.util
            }

            Connections {
                function onRequestRaise() {
                    root.show()
                }

                target: App.screenPlayManager
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

    Item {
        width: 15
        height: width
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 1
        }
        Image {
            source: "qrc:/qml/ScreenPlayApp/assets/images/scale_window_indicator.png"
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
