import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import "../Components"

Rectangle {
    id: root

    property string currentNavigationName: "Installed"
    property var navArray: [navCreate, navWorkshop, navInstalled, navCommunity, navSettings]
    property bool navActive: true
    property Item modalSource
    property int iconWidth: 16
    property int iconHeight: iconWidth
    property bool isSmallScreen: width < 1366

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
    implicitHeight: 60
    color: Material.theme === Material.Light ? "white" : Material.background
    layer.enabled: true
    layer.effect: ElevationEffect {
        elevation: 2
    }

    Connections {
        function onRequestNavigationActive(isActive) {
            root.setActive(isActive);
        }

        function onRequestNavigation(nav) {
            root.onPageChanged(nav);
        }

        target: App.util
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
    RowLayout {
        spacing: 10
        anchors {
            fill: parent
            leftMargin: 10
            rightMargin: 10
        }

        TabBar {
            id: tabBar
            currentIndex: 2
            Layout.fillHeight: true
            Layout.topMargin: 8

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
        Item {
            id: spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Control {
            id: premium
            Layout.maximumHeight: 45
            visible: true
            background: Rectangle {
                color: Material.theme === Material.Light ? Material.backgroundColor : "#242424"
                border.color: "#44FFD700"
                border.width: 1
                radius: 3
            }

            ScreenPlayProPopup {
                id: screenPlayProView
            }

            contentItem: ToolButton {
                id: btnPremiumPopup
                height: 45
                icon.source: "qrc:/qml/ScreenPlayApp/assets/images/rocket_3d.png"
                icon.color: "transparent"
                text: {
                    if (root.isSmallScreen) {
                        return qsTr("Go Pro");
                    }
                    if (App.globalVariables.isBasicVersion()) {
                        return qsTr("Get ScreenPlay Pro 3D Wallpaper and Timelines!");
                    }
                    if (App.globalVariables.isProVersion()) {
                        return qsTr("ScreenPlay Pro Active");
                    }
                    if (App.globalVariables.isUltraVersion()) {
                        return qsTr("ScreenPlay ULTRA Active");
                    }
                }

                onClicked: {
                    if (App.globalVariables.isBasicVersion()) {
                        screenPlayProView.open();
                    }
                }
            }
        }

        Control {
            Layout.maximumHeight: 45
            background: Rectangle {
                id: quickActionRowBackground
                color: Material.theme === Material.Light ? Material.backgroundColor : "#242424"
                border.color: Material.theme === Material.Light ? Material.iconDisabledColor : Qt.darker(Material.background)
                border.width: 1
                radius: 3
            }

            contentItem: RowLayout {
                id: quickActionRow
                height: 45
                spacing: 0

                property bool contentActive: App.screenPlayManager.activeWallpaperCounter > 0 || App.screenPlayManager.activeWidgetsCounter > 0

                onContentActiveChanged: {
                    if (!contentActive) {
                        miMuteAll.soundEnabled = true;
                        miStopAll.isPlaying = true;
                    }
                }

                ToolButton {
                    id: miMuteAll
                    height: 45
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
                    height: 45
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
                    height: 45
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
        }

        Button {
            id: miConfig
            Layout.maximumHeight: 55
            Layout.minimumHeight: 55
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_video_settings.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            onClicked: App.util.setToggleWallpaperConfiguration()
            hoverEnabled: true
            text: !root.isSmallScreen ? (contentActive ? qsTr("Configure Content") : qsTr("No Active Content")) : ""

            Material.accent: contentActive ? "gold" : Material.secondaryTextColor
            property bool contentActive: App.screenPlayManager.activeWallpaperCounter > 0

            background: Rectangle {
                color: Material.theme === Material.Light ? Material.backgroundColor : "#242424"
                border.color: miConfig.contentActive ? "gold" : (Material.theme === Material.Light ? Material.iconDisabledColor : Qt.darker(Material.background))
                border.width: 1
                radius: 3
                height: 55
            }

            ToolTip {
                visible: miConfig.hovered
                text: miConfig.contentActive ? qsTr("Configure Content") : qsTr("No Active Content")
            }
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
