import QtQuick
import QtQuick.Controls
import ScreenPlayCore
import ScreenPlay
import "ContentSettings" as ContentSettings
import "Navigation" as Navigation

Item {
    id: content
    anchors.fill: parent
    property ApplicationWindow applicationWindow
    Component.onCompleted: {
        stackView.push("qrc:/qt/qml/ScreenPlay/qml/Installed/InstalledView.qml");
    }

    function openExitDialog(): void {
        exitDialog.open();
    }
    function switchPage(name: string): void {
        stackView.replace(`qrc:/qt/qml/ScreenPlay/qml/${name}/${name}View.qml`, {
            "modalSource": content
        });
        nav.setNavigation(name);
    }

    Navigation.ExitPopup {
        id: exitDialog
        applicationWindow: content.applicationWindow
        // modalSource: content
    }

    Item {
        id: noneContentItems
        SteamNotAvailable {
            id: dialogSteam
            // modalSource: content
        }

        MonitorConfiguration {
            // modalSource: content
        }

        CriticalError {
            applicationWindow: content.applicationWindow
            // modalSource: content
        }

        ContentSettings.ContentSettingsView {
            id: contentSettingsView
            // modalSource: content
        }
        TrayIcon {
            applicationWindow: content.applicationWindow
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
            content.applicationWindow.show();
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
            content.switchPage(name);
        }
    }
}
