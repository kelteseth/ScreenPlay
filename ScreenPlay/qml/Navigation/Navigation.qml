import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlay 1.0
import "../Workshop"
import "../Common"

Rectangle {
    id: root

    property string currentNavigationName: "Installed"
    property var navArray: [navCreate, navWorkshop, navInstalled, navSettings, navCommunity]
    property bool navActive: true
    property ApplicationWindow window
    property var modalSource
    property int iconWidth: 16
    property int iconHeight: iconWidth


    signal changePage(string name)

    function setActive(active) {
        navActive = active
        if (active)
            root.state = "enabled"
        else
            root.state = "disabled"
    }

    function setNavigation(name) {
        var i = 0
        for (; i < navArray.length; i++) {
            if (navArray[i].name === name) {
                navArray[i].state = "active"
                root.currentNavigationName = name
            } else {
                navArray[i].state = "inactive"
            }
        }
    }

    function onPageChanged(name) {
        if (!navActive)
            return

        root.changePage(name)
        setNavigation(name)
    }

    height: 89
    width: 1366
    color: Material.theme === Material.Light ? "white" : Material.background
    layer.enabled: true

    Rectangle {
        id:navBg
        height:29
        width:parent.width
        color: Material.theme === Material.Light ? Material.background : "#242424"

        Text {
            id: title
            text: qsTr("ScreenPlay Alpha %1  - Open Source Wallpaper And Widgets").arg(ScreenPlay.version())
            color: Material.primaryTextColor
            anchors{
                left:parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }
    }

    MouseArea {
        id: mouseArea

        property var clickPos

        anchors.fill: parent
        hoverEnabled: true
        onPressed: (mouse)=>{
            clickPos = {
                "x": mouse.x,
                "y": mouse.y
            };
        }
        onPositionChanged: {
            if (mouseArea.pressed){
                let pos = ScreenPlay.cursorPos();
                window.setX(pos.x - clickPos.x)
                window.setY(pos.y - clickPos.y)
            }

        }
    }

    RowLayout {
        anchors {
            top:navBg.top
            right: navBg.right
            bottom: navBg.bottom
        }

        WindowNavButton {
            id: miMinimize
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/assets/icons/icon_minimize.svg"
            onClicked: root.window.hide()
        }
        WindowNavButton {
            id: miquit
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/assets/icons/icon_close.svg"
            onClicked: {
                if(ScreenPlay.screenPlayManager.activeWallpaperCounter === 0
                        && ScreenPlay.screenPlayManager.activeWidgetsCounter === 0){
                    Qt.quit()
                    return
                }
                dialog.open()
            }
        }
    }

    Connections {
        function onRequestNavigationActive(isActive) {
            setActive(isActive)
        }

        function onRequestNavigation(nav) {
            onPageChanged(nav)
        }

        target: ScreenPlay.util
    }




    Row {
        id: row
        height: 60

        anchors {
            top:parent.top
            topMargin: navBg.height
            right: parent.right
            left: parent.left
            leftMargin: 20
        }

        spacing: 0

        NavigationItem {
            id: navCreate

            state: "inactive"
            name: "Create"
            text: qsTr("Create")
            iconSource: "qrc:/assets/icons/icon_plus.svg"
            onPageClicked: (name)=> {root.onPageChanged(name)}
            objectName: "createTab"
        }

        NavigationItem {
            id: navWorkshop

            state: "inactive"
            name: "Workshop"
            text: qsTr("Workshop")
            iconSource: "qrc:/assets/icons/icon_steam.svg"
            onPageClicked: (name)=> {root.onPageChanged(name)}
            objectName: "workshopTab"
        }

        NavigationItem {
            id: navInstalled

            state: "active"
            name: "Installed"
            text: qsTr("Installed")
            amount: ScreenPlay.installedListModel.count
            iconSource: "qrc:/assets/icons/icon_installed.svg"
            onPageClicked: (name)=> {root.onPageChanged(name)}
            objectName: "installedTab"
        }

        NavigationItem {
            id: navCommunity

            state: "inactive"
            name: "Community"
            text: qsTr("Community")
            iconSource: "qrc:/assets/icons/icon_community.svg"
            onPageClicked: (name)=> {root.onPageChanged(name)}
            objectName: "communityTab"
        }

        NavigationItem {
            id: navSettings

            state: "inactive"
            name: "Settings"
            text: qsTr("Settings")
            iconSource: "qrc:/assets/icons/icon_settings.svg"
            onPageClicked: (name)=> {root.onPageChanged(name)}
            objectName: "settingsTab"
        }
    }

    Rectangle {
        id:quickActionRowBackground
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
            topMargin: navBg.height
            right: quickActionRow.left
            rightMargin: 20
            bottom: parent.bottom
        }

        ToolButton {
            icon.source: "qrc:/assets/icons/font-awsome/patreon-brands.svg"
            text: qsTr("Support me on Patreon!")
            onClicked: Qt.openUrlExternally("https://www.patreon.com/ScreenPlayApp")
        }
    }

    RowLayout {
        id: quickActionRow
        anchors {
            top: parent.top
            topMargin: navBg.height
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
        }

        property bool contentActive: ScreenPlay.screenPlayManager.activeWallpaperCounter > 0
                                     || ScreenPlay.screenPlayManager.activeWidgetsCounter > 0

        onContentActiveChanged: {
            if(!contentActive){
                miMuteAll.isMuted = false
                miStopAll.isPlaying = false
            }
        }

        ToolButton {
            id: miMuteAll
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/assets/icons/icon_volume.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            enabled: quickActionRow.contentActive

            onClicked: isMuted = !isMuted
            property bool isMuted: false
            onIsMutedChanged: {
                if (miMuteAll.isMuted) {
                    miMuteAll.icon.source = "qrc:/assets/icons/icon_volume.svg"
                    ScreenPlay.screenPlayManager.setAllWallpaperValue("muted",
                                                                      "false")
                } else {
                    miMuteAll.icon.source = "qrc:/assets/icons/icon_volume_mute.svg"
                    ScreenPlay.screenPlayManager.setAllWallpaperValue("muted",
                                                                      "true")
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
            icon.source: "qrc:/assets/icons/icon_pause.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            onClicked: isPlaying = !isPlaying
            property bool isPlaying: true
            onIsPlayingChanged:{
                if (miStopAll.isPlaying) {
                    miStopAll.icon.source = "qrc:/assets/icons/icon_pause.svg"
                    ScreenPlay.screenPlayManager.setAllWallpaperValue(
                                "isPlaying", "true")
                } else {
                    miStopAll.icon.source = "qrc:/assets/icons/icon_play.svg"
                    ScreenPlay.screenPlayManager.setAllWallpaperValue(
                                "isPlaying", "false")
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
            icon.source: "qrc:/assets/icons/icon_close.svg"
            icon.width: root.iconWidth
            icon.height: root.iconHeight
            icon.color: Material.iconColor
            onClicked: {
                ScreenPlay.screenPlayManager.removeAllWallpapers()
                ScreenPlay.screenPlayManager.removeAllWidgets()
            }

            hoverEnabled: true
            ToolTip.text: qsTr("Close All Content")
            ToolTip.visible: hovered
        }

        ToolButton {
                id: miConfig
                Layout.alignment: Qt.AlignVCenter
                icon.source: "qrc:/assets/icons/icon_video_settings_black_24dp.svg"
                icon.width: root.iconWidth
                icon.height: root.iconHeight
                onClicked: ScreenPlay.util.setToggleWallpaperConfiguration()
                hoverEnabled: true
                ToolTip.text: qsTr("Configure Wallpaper")
                ToolTip.visible: hovered
            }





        Dialog {
            id: dialog
            anchors.centerIn: Overlay.overlay

            Overlay.modal: ModalBackgroundBlur {
                sourceItem: root.modalSource
            }
            title: qsTr("Are you sure you want to exit ScreenPlay? \nThis will shut down all Wallpaper and Widgets.")
            standardButtons: Dialog.Ok | Dialog.Cancel
            onAccepted: Qt.quit()
            modal: true
        }


    }

    layer.effect: ElevationEffect {
        elevation: 2
    }

    states: [
        State {
            name: "enabled"
        },
        State {
            name: "disabled"

            PropertyChanges {
                target: row
                opacity: 0.3
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"

            PropertyAnimation {
                target: row
                duration: 300
            }
        }
    ]
}
