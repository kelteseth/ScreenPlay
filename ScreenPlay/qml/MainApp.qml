import QtQuick
import QtQuick.Controls
import ScreenPlayUtil
import ScreenPlayApp 1.0
import "ContentSettings" as ContentSettings
import "Navigation" as Navigation

Item {
    id: content
    anchors.fill: parent
    Component.onCompleted: {
        setTheme(App.settings.theme);
        stackView.push("qrc:/qml/ScreenPlayApp/qml/Installed/InstalledView.qml");
        startTimer.start();
    }

    function openExitDialog() {
        exitDialog.open();
    }

    Timer {
        id: startTimer
        interval: 10
        onTriggered: App.installedListModel.reset()
    }

    function switchPage(name) {
        if (nav.currentNavigationName === name) {
            if (name === "Installed")
                App.installedListModel.reset();
        }
        stackView.replace("qrc:/qml/ScreenPlayApp/qml/" + name + "/" + name + "View.qml", {
            "modalSource": content
        });
        nav.setNavigation(name);
    }

    Navigation.ExitPopup {
        id: exitDialog
        applicationWindow: root
        modalSource: content
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

        ContentSettings.ContentSettingsView {
            id: contentSettingsView
            modalSource: content
        }
        TrayIcon {
            window: root
        }
    }

    function setTheme(theme) {
        switch (theme) {
        case Settings.Theme.System:
            root.Material.theme = Material.System;
            break;
        case Settings.Theme.Dark:
            root.Material.theme = Material.Dark;
            break;
        case Settings.Theme.Light:
            root.Material.theme = Material.Light;
            break;
        }
    }

    Connections {
        function onThemeChanged(theme) {
            content.setTheme(theme);
        }

        target: App.settings
    }

    Connections {
        function onRequestNavigation(nav) {
            content.switchPage(nav);
        }

        target: App.util
    }

    Connections {
        function onRequestRaise() {
            App.showDockIcon(true);
            window.show();
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
    Navigation.Navigation {
        id: nav
        modalSource: content
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        onChangePage: function (name) {
            // Close in case the user clicks
            // on the tray icon
            contentSettingsView.close();
            switchPage(name);
        }
    }
}
