import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlay
import ScreenPlayUtil

Rectangle {
    id: root

    property string currentNavigationName: "Installed"
    property var navArray: [navCreate, navWorkshop, navInstalled, navCommunity, navSettings]
    property bool navActive: true
    property Item modalSource
    property int iconWidth: 16
    property int iconHeight: iconWidth

    signal changePage(string name)

    function setActive(active) {
        navActive = active;
        if (active)
            root.state = "enabled";
        else
            root.state = "disabled";
    }
    function setNavigation(name) {
        for (var i = 0; i < navArray.length; i++) {
            if (navArray[i].objectName === name) {
                navArray[i].state = "active";
                root.currentNavigationName = name;
                tabBar.currentIndex = navArray[i].index;
            } else {
                navArray[i].state = "inactive";
            }
        }
    }

    function onPageChanged(name) {
        if (!navActive)
            return;
        root.changePage(name);
        setNavigation(name);
    }

    implicitWidth: 1366
    height: 60
    width: 1366
    color: Material.theme === Material.Light ? "white" : Material.background
    layer.enabled: true
    layer.effect: ElevationEffect {
        elevation: 2
    }

    Connections {
        function onRequestNavigationActive(isActive) {
            setActive(isActive);
        }

        function onRequestNavigation(nav) {
            onPageChanged(nav);
        }

        target: App.util
    }

    TabBar {
        id: tabBar
        height: 50
        currentIndex: 2

        anchors {
            bottom: parent.bottom
            left: parent.left
            leftMargin: 20
        }

        spacing: 0

        CustomTabButton {
            id: navCreate
            index: 0
            icon.height: 22
            icon.width: 22
            text: qsTr("Create")
            objectName: "Create"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_plus.svg"
            onClicked: {
                root.onPageChanged("Create");
            }
        }

        CustomTabButton {
            id: navWorkshop
            index: 1
            enabled: App.settings.steamVersion
            text: qsTr("Workshop")
            objectName: "Workshop"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_steam.svg"
            onClicked: {
                root.onPageChanged("Workshop");
            }
        }

        CustomTabButton {
            id: navInstalled
            index: 2
            text: qsTr("Installed") + " " + App.installedListModel.count
            objectName: "Installed"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_installed.svg"
            onClicked: {
                root.onPageChanged("Installed");
            }
        }

        CustomTabButton {
            id: navCommunity
            index: 3
            text: qsTr("Community")
            objectName: "Community"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_community.svg"
            onClicked: {
                root.onPageChanged("Community");
            }
        }

        CustomTabButton {
            id: navSettings
            index: 4
            text: qsTr("Settings")
            objectName: "Settings"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_settings.svg"
            onClicked: {
                root.onPageChanged("Settings");
            }
        }
    }

    component CustomTabButton: TabButton {
        icon.height: 16
        icon.width: 16
        font.pointSize: 12
        height: parent.height
        width: implicitWidth
        property int index: 0
        background: Item {
        }
        font.capitalization: Font.MixedCase
    }

    Rectangle {
        id: quickActionRowBackground
        anchors.centerIn: quickActionRow
        width: quickActionRow.width + 5
        height: quickActionRow.height - 16
        color: Material.theme === Material.Light ? Material.background : "#242424"
        border.color: Material.theme === Material.Light ? Material.iconDisabledColor : Qt.darker(Material.background)
        border.width: 1
        radius: 3
    }

    RowLayout {
        anchors {
            top: parent.top
            right: quickActionRow.left
            rightMargin: 20
            bottom: parent.bottom
        }

        ToolButton {
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/font-awsome/patreon-brands.svg"
            text: qsTr("Support me on Patreon!")
            onClicked: Qt.openUrlExternally("https://www.patreon.com/ScreenPlayApp")
        }
    }

    RowLayout {
        id: quickActionRow
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
        }

        property bool contentActive: App.screenPlayManager.activeWallpaperCounter > 0 || App.screenPlayManager.activeWidgetsCounter > 0

        onContentActiveChanged: {
            if (!contentActive) {
                miMuteAll.soundEnabled = true;
                miStopAll.isPlaying = true;
            }
        }

        ToolButton {
            id: miMuteAll
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_volume.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            enabled: quickActionRow.contentActive

            onClicked: soundEnabled = !soundEnabled
            property bool soundEnabled: true
            onSoundEnabledChanged: {
                if (miMuteAll.soundEnabled) {
                    miMuteAll.icon.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_volume.svg";
                    App.screenPlayManager.setAllWallpaperValue("muted", "false");
                } else {
                    miMuteAll.icon.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_volume_mute.svg";
                    App.screenPlayManager.setAllWallpaperValue("muted", "true");
                }
            }

            hoverEnabled: true
            ToolTip.text: qsTr("Mute/Unmute all Wallpaper")
            ToolTip.visible: hovered
        }
        ToolButton {
            id: miStopAll
            enabled: quickActionRow.contentActive
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_pause.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            onClicked: isPlaying = !isPlaying
            property bool isPlaying: true
            onIsPlayingChanged: {
                if (miStopAll.isPlaying) {
                    miStopAll.icon.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_pause.svg";
                    App.screenPlayManager.setAllWallpaperValue("isPlaying", "true");
                } else {
                    miStopAll.icon.source = "qrc:/qml/ScreenPlayApp/assets/icons/icon_play.svg";
                    App.screenPlayManager.setAllWallpaperValue("isPlaying", "false");
                }
            }
            hoverEnabled: true
            ToolTip.text: qsTr("Pause/Play all Wallpaper")
            ToolTip.visible: hovered
        }

        ToolButton {
            id: miCloseAll
            enabled: quickActionRow.contentActive
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_close.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            onClicked: {
                App.screenPlayManager.removeAllWallpapers();
                App.screenPlayManager.removeAllWidgets();
                miStopAll.isPlaying = true;
                miMuteAll.soundEnabled = true;
            }

            hoverEnabled: true
            ToolTip.text: qsTr("Close All Content")
            ToolTip.visible: hovered
        }

        ToolButton {
            id: miConfig
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_video_settings_black_24dp.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            onClicked: App.util.setToggleWallpaperConfiguration()
            hoverEnabled: true
            ToolTip.text: qsTr("Configure Wallpaper")
            ToolTip.visible: hovered
        }
    }

    states: [
        State {
            name: "enabled"
        },
        State {
            name: "disabled"

            PropertyChanges {
                target: tabBar
                opacity: 0.3
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"

            PropertyAnimation {
                target: tabBar
                duration: 300
            }
        }
    ]
}
