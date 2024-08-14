import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlayUtil
import "../Components"

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
            enabled: App.globalVariables.isSteamVersion()
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
        background: Item {}
        font.capitalization: Font.MixedCase
    }

    Rectangle {
        id: premiumBackground
        anchors.centerIn: premium
        width: premium.width
        height: premium.height
        color: Material.theme === Material.Light ? Material.background : "#242424"
        border.color: "#44FFD700"
        border.width: 1
        radius: 3
    }

    SwipeView {
        id: premium
        clip: true
        anchors {
            top: parent.top
            topMargin: 5
            right: quickActionRow.left
            rightMargin: 20
            bottom: parent.bottom
            bottomMargin: 5
        }
        interactive: false

        currentIndex: {
            if (App.globalVariables.isBasicVersion())
                return 0;
            if (App.globalVariables.isProVersion())
                return 1;
            if (App.globalVariables.isUltraVersion())
                return 2;
            return 0;
        }

        ScreenPlayProPopup {
            id: screenPlayProView
        }

        ToolButton {
            icon.source: "qrc:/qml/ScreenPlayApp/assets/images/rocket_3d.png"
            icon.color: "transparent"
            text: qsTr("Get ScreenPlay Pro 3D Wallpaper and Timelines!")
            onClicked: screenPlayProView.open()
        }
        ToolButton {
            text: qsTr("ScreenPlay Pro Active")
        }
        ToolButton {
            text: qsTr("ScreenPlay ULTRA Active")
        }
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
        id: quickActionRow
        anchors {
            top: parent.top
            right: miConfig.left
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
                App.screenPlayManager.removeAllRunningWidgets(false);
                // Saves profiles.json
                App.screenPlayManager.removeAllRunningWallpapers(true);
                miStopAll.isPlaying = true;
                miMuteAll.soundEnabled = true;
            }

            hoverEnabled: true
            ToolTip.text: qsTr("Stop All Running Content")
            ToolTip.visible: hovered
        }
    }

    Button {
        id: miConfig
        icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_video_settings.svg"
        icon.width: root.iconWidth
        icon.height: root.iconHeight
        onClicked: App.util.setToggleWallpaperConfiguration()
        hoverEnabled: true
        text: contentActive ? qsTr("Configure Content") : qsTr("No Active Content")
        bottomInset: 10
        topInset: 10
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
        }

        Material.accent: contentActive ? "gold" : Material.secondaryTextColor
        property bool contentActive: App.screenPlayManager.activeWallpaperCounter > 0

        background: Rectangle {
            color: Material.theme === Material.Light ? Material.background : "#242424"
            border.color: {
                if (miConfig.contentActive) {
                    return "gold";
                } else {
                    Material.theme === Material.Light ? Material.iconDisabledColor : Qt.darker(Material.background);
                }
            }

            border.width: 1
            radius: 3
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
