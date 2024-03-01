import QtQuick
import QtQuick.Controls
import ScreenPlayUtil as Util
import ScreenPlayApp 1.0
import "Monitors" as Monitors
import "Installed" as Installed
import "Navigation" as Navigation
import "Community" as Community

Item {
    id: content
    anchors.fill: parent
    Component.onCompleted: {
        stackView.push(
                    "qrc:/qml/ScreenPlayApp/qml/Installed/InstalledView.qml", {
                        "sidebar": sidebar
                    })
        startTimer.start()
    }
    Timer {
        id: startTimer
        interval: 10
        onTriggered:  App.installedListModel.reset()
    }


    function switchPage(name) {

        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                App.installedListModel.reset();
        }
        if (name === "Installed") {
            stackView.replace("qrc:/qml/ScreenPlayApp/qml/Installed/InstalledView.qml", {
                    "sidebar": sidebar
                });
            return;
        }
        stackView.replace("qrc:/qml/ScreenPlayApp/qml/" + name + "/" + name + "View.qml", {
                "modalSource": content
            });
        nav.setNavigation(name);
        sidebar.state = "inactive";
    }

    Navigation.ExitPopup {
        id: exitDialog
        applicationWindow: root
        modalSource: content
    }

    Item {
        id: noneContentItems
        Util.SteamNotAvailable {
            id: dialogSteam
            modalSource: content
        }

        Util.MonitorConfiguration {
            modalSource: content
        }

        Util.CriticalError {
            window: root
            modalSource: content
        }

        Monitors.MonitorsView {
            id: monitors
            modalSource: content
        }
        TrayIcon {
            window: root
        }
    }

    Connections {
        function onThemeChanged(theme) {
            setTheme(theme);
        }

        target: App.settings
    }

    Connections {
        function onRequestNavigation(nav) {
            switchPage(nav);
        }

        target: App.util
    }

    Connections {
        function onRequestRaise() {
            App.showDockIcon(true);
            root.show();
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
                sidebar.state = "active";
            else
                sidebar.state = "inactive";
        }

        function onSetNavigationItem(pos) {
            if (pos === 0)
                nav.onPageChanged("Create");
            else
                nav.onPageChanged("Workshop");
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
            monitors.close();
            switchPage(name);
        }
    }
}
