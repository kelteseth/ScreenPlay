import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlayUtil as Util

import "../Components"

Util.Popup {
    id: root

    property string activeMonitorName: ""
    property int activeMonitorIndex

    width: 1366
    height: 768
    onOpened: {
        monitorSelection.selectMonitorAt(0);
        timeline.reset();
    }
    background: Rectangle {
        anchors.fill: parent
        radius: 4
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 6
        }
    }
    Connections {
        function onRequestToggleWallpaperConfiguration() {
            root.open();
        }

        target: App.util
    }
    ColumnLayout {

        anchors {
            fill: parent
            margins: 10
        }

        Rectangle {
            id: timelineWrapper
            color: Material.theme === Material.Light ? Material.background : "#242424"
            border.color: "#44FFD700"
            border.width: 1
            radius: 3
            Layout.fillWidth: true
            Layout.preferredHeight: 240

            ColumnLayout {
                spacing: 10

                anchors {
                    fill: parent
                    margins: 20
                }

                Text {
                    text: qsTr("Timeline")
                    font.pointSize: 21
                    color: Material.primaryTextColor
                    font.family: App.settings.font
                    font.weight: Font.Light
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                }

                Timeline {
                    id: timeline
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        Item {
            id: monitorsSettingsWrapper

            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                id: itmLeftWrapper

                width: parent.width * 0.5

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
                    font.family: App.settings.font
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

                    bgRadius: 3
                    height: 200
                    width: parent.width * 0.9
                    multipleMonitorsSelectable: false
                    monitorWithoutContentSelectable: false
                    onRequestProjectSettings: function (index, installedType, appID) {
                        if (installedType === Util.ContentTypes.InstalledType.VideoWallpaper) {
                            videoControlWrapper.state = "visible";
                            customPropertiesGridView.visible = false;
                            const wallpaper = App.screenPlayManager.getWallpaperByAppID(appID);
                            videoControlWrapper.wallpaper = wallpaper;
                        } else {
                            videoControlWrapper.state = "hidden";
                            customPropertiesGridView.visible = true;
                            if (!App.screenPlayManager.requestProjectSettingsAtMonitorIndex(index)) {
                                console.warn("Unable to get requested settings from index: ", index);
                            }
                        }
                        activeMonitorIndex = index;
                    }
                    onRequestRemoveWallpaper: index => {
                        const selectedTimeline = timeline.getSelectedTimeline();
                        if (selectedTimeline === undefined) {
                            console.error("No active timeline to remove wallpaper ", index);
                            return;
                        }
                        monitorSelection.enabled = false;
                        App.screenPlayManager.removeWallpaperAt(index, selectedTimeline.identifier, selectedTimeline.index).then(result => {
                            monitorSelection.enabled = true;
                            if (!result.success) {} else {}
                        });
                    }

                    anchors {
                        top: txtHeadline.bottom
                        topMargin: 20
                        left: parent.left
                        leftMargin: 20
                    }

                    Connections {
                        function onProjectSettingsListModelResult(listModel) {
                            customPropertiesGridView.projectSettingsListmodelRef = listModel;
                        }

                        target: App.screenPlayManager
                    }
                }

                ColumnLayout {
                    spacing: 5

                    anchors {
                        top: monitorSelection.bottom
                        right: parent.right
                        left: parent.left
                        margins: 20
                    }

                    Button {
                        id: btnRemoveSelectedWallpaper
                        Material.background: Material.accent
                        highlighted: true
                        text: qsTr("Remove selected")
                        font.family: App.settings.font
                        enabled: monitorSelection.activeMonitors.length == 1 && App.screenPlayManager.activeWallpaperCounter > 0
                        onClicked: {
                            if (!App.screenPlayManager.removeWallpaperAt(monitorSelection.activeMonitors[0]))
                                print("Unable to close singel wallpaper");
                        }
                    }

                    Button {
                        id: btnRemoveAllWallpape

                        text: qsTr("Remove all ") + App.screenPlayManager.activeWallpaperCounter + " " + qsTr("Wallpapers")
                        Material.background: Material.accent
                        highlighted: true
                        font.family: App.settings.font
                        enabled: App.screenPlayManager.activeWallpaperCounter > 0
                        onClicked: {
                            if (!App.screenPlayManager.removeAllRunningWallpapers(true))
                                print("Unable to close all wallpaper!");
                        }
                    }

                    Button {
                        id: btnRemoveAllWidgets

                        text: qsTr("Remove all ") + App.screenPlayManager.activeWidgetsCounter + " " + qsTr("Widgets")
                        Material.background: Material.accent
                        Material.foreground: Material.primaryTextColor
                        highlighted: true
                        font.family: App.settings.font
                        enabled: App.screenPlayManager.activeWidgetsCounter > 0
                        onClicked: {
                            if (!App.screenPlayManager.removeAllRunningWidgets())
                                print("Unable to close all widgets!");
                        }
                    }
                }
            }

            Rectangle {
                color: Material.theme === Material.Light ? Material.background : Qt.darker(Material.background)
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

                    activeMonitorIndex: root.activeMonitorIndex
                    state: "hidden"
                    anchors.fill: parent
                    anchors.margins: 10
                }

                GridView {
                    id: customPropertiesGridView

                    property var projectSettingsListmodelRef

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

            SaveNotification {
                id: saveNotification

                width: parent.width - 40

                Connections {
                    function onProfilesSaved() {
                        if (root.opened)
                            saveNotification.open();
                    }

                    target: App.screenPlayManager
                }
            }
        }
    }
    ToolButton {
        width: 32
        height: width
        icon.width: 16
        icon.height: 16
        icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/font-awsome/close.svg"
        icon.color: Material.iconColor
        onClicked: root.close()

        anchors {
            top: parent.top
            right: parent.right
        }
    }
}
