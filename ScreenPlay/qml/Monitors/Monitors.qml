import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0

import ScreenPlay.Enums.InstalledType 1.0
import "../Common/" as SP

Popup {
    id: monitors
    width: 1000
    height: 500
    dim: true
    anchors.centerIn: Overlay.overlay

    modal: true
    focus: true
    background: Rectangle {
        anchors.fill: parent
        radius: 4
        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 6
        }
        color: Material.theme === Material.Light ? "white" : Material.background
    }

    property string activeMonitorName: ""
    property int activeMonitorIndex

    Connections {
        target: ScreenPlay.util
        function onRequestToggleWallpaperConfiguration() {
            monitors.open()
        }
    }

    onOpened: {
        monitorSelection.selectMonitorAt(0)
    }

    Item {
        id: monitorsSettingsWrapper

        clip: true
        anchors {
            fill: parent
            margins: 10
        }

        Item {
            id: itmLeftWrapper
            width: parent.width * .5
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 10
            }
            Text {
                id: txtHeadline
                text: qsTr("Wallpaper Configuration")
                font.pointSize: 21
                color: Material.primaryTextColor
                font.family: ScreenPlay.settings.font
                font.weight: Font.Light
                width: 400
                anchors {
                    top: parent.top
                    topMargin: 10
                    left: parent.left
                    leftMargin: 20
                }
            }
            MonitorSelection {
                id: monitorSelection
                radius: 3
                height: 200
                width: parent.width * .9
                multipleMonitorsSelectable: false
                monitorWithoutContentSelectable: false
                anchors {
                    top: txtHeadline.bottom
                    topMargin: 20
                    left: parent.left
                    leftMargin: 20
                }
                availableWidth: width - 20
                availableHeight: 150

                onRequestProjectSettings: {

                    if (installedType === InstalledType.VideoWallpaper) {
                        videoControlWrapper.state = "visible"
                        customPropertiesGridView.visible = false
                        const wallpaper = ScreenPlay.screenPlayManager.getWallpaperByAppID(appID)
                        videoControlWrapper.wallpaper = wallpaper
                    } else {
                        videoControlWrapper.state = "hidden"
                        customPropertiesGridView.visible = true
                        ScreenPlay.screenPlayManager.requestProjectSettingsAtMonitorIndex(index)
                    }

                    activeMonitorIndex = index
                }

                Connections {
                    target: ScreenPlay.screenPlayManager
                    function onProjectSettingsListModelResult(listModel) {
                        customPropertiesGridView.projectSettingsListmodelRef = listModel
                    }
                }
            }

            ColumnLayout {
                anchors {
                    top: monitorSelection.bottom
                    right: parent.right
                    left: parent.left
                    margins: 20
                }
                spacing: 5
                Button {
                    id: btnRemoveSelectedWallpaper
                    text: qsTr("Remove selected")
                    Material.background: Material.accent
                    Material.foreground: "white"
                    font.family: ScreenPlay.settings.font
                    enabled: monitorSelection.activeMonitors.length == 1
                    onClicked: {
                        if (!ScreenPlay.screenPlayManager.removeWallpaperAt(
                                    monitorSelection.activeMonitors[0])) {
                            print("Unable to close singel wallpaper")
                        }
                    }
                }
                Button {
                    id: btnRemoveAllWallpape
                    text: qsTr("Remove ")
                          + ScreenPlay.screenPlayManager.activeWallpaperCounter + " " + qsTr(
                              "Wallpapers")
                    Material.background: Material.accent
                    Material.foreground: "white"
                    font.family: ScreenPlay.settings.font
                    enabled: ScreenPlay.screenPlayManager.activeWallpaperCounter > 0
                    onClicked: {
                        if (!ScreenPlay.screenPlayManager.removeAllWallpapers()) {
                            print("Unable to close all wallpaper!")
                        }

                        monitors.close()
                    }
                }
                Button {
                    id: btnRemoveAllWidgets
                    text: qsTr("Remove ")
                          + ScreenPlay.screenPlayManager.activeWidgetsCounter + " " + qsTr("Widgets")
                    Material.background: Material.accent
                    Material.foreground: "white"
                    font.family: ScreenPlay.settings.font
                    enabled: ScreenPlay.screenPlayManager.activeWidgetsCounter > 0
                    onClicked: {
                        if (!ScreenPlay.screenPlayManager.removeAllWidgets()) {
                            print("Unable to close all widgets!")
                        }

                        monitors.close()
                    }
                }
            }
        }

        Rectangle {
            color: Material.theme === Material.Light ? Material.background : Qt.darker(
                                                           Material.background)
            radius: 3
            clip: true

            anchors {
                top: parent.top
                topMargin: 75
                right: parent.right
                rightMargin: 40
                bottom: parent.bottom
                bottomMargin: 30
                left: itmLeftWrapper.right
            }

            DefaultVideoControls {
                id: videoControlWrapper
                activeMonitorIndex: monitors.activeMonitorIndex
                state: "hidden"
                anchors.fill: parent
                anchors.margins: 10
            }

            GridView {
                id: customPropertiesGridView
                boundsBehavior: Flickable.DragOverBounds
                maximumFlickVelocity: 7000
                flickDeceleration: 5000
                cellWidth: 340
                cellHeight: 50
                cacheBuffer: 10000
                clip: true
                anchors.fill: parent
                anchors.margins: 10
                visible: false
                model: customPropertiesGridView.projectSettingsListmodelRef
                property var projectSettingsListmodelRef

                delegate: MonitorsProjectSettingItem {
                    id: delegate
                    width: parent.width - 40
                    selectedMonitor: activeMonitorIndex
                    name: m_name
                    isHeadline: m_isHeadline
                    value: m_value
                    itemIndex: index
                    projectSettingsListmodelRef: customPropertiesGridView.projectSettingsListmodelRef
                }

                ScrollBar.vertical: ScrollBar {
                    snapMode: ScrollBar.SnapOnRelease
                    policy: ScrollBar.AlwaysOn
                }
            }
        }

        ToolButton {
            anchors {
                top: parent.top
                right: parent.right
            }
            width: 32
            height: width
            icon.width: 16
            icon.height: 16
            icon.source: "qrc:/assets/icons/font-awsome/close.svg"
            icon.color: Material.iconColor
            onClicked: monitors.close()
        }

        SaveNotification {
            id: saveNotification
            width: parent.width - 40
            Connections {
                target: ScreenPlay.screenPlayManager
                function onProfilesSaved() {
                    if (monitors.opened)
                        saveNotification.open()
                }
            }
        }
    }
}
