import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Enums.InstalledType
import ScreenPlayUtil as Util

Util.Popup {
    id: root

    property string activeMonitorName: ""
    property int activeMonitorIndex

    width: 1000
    height: 500
    onOpened: {
        monitorSelection.selectMonitorAt(0);
    }

    Connections {
        function onRequestToggleWallpaperConfiguration() {
            root.open();
        }

        target: App.util
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

                radius: 3
                height: 200
                width: parent.width * 0.9
                multipleMonitorsSelectable: false
                monitorWithoutContentSelectable: false
                availableWidth: width - 20
                availableHeight: 150
                onRequestProjectSettings: function (index, installedType, appID) {
                    if (installedType === InstalledType.VideoWallpaper) {
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
                        if (!App.screenPlayManager.removeAllWallpapers())
                            print("Unable to close all wallpaper!");
                        root.close();
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
                        if (!App.screenPlayManager.removeAllWidgets())
                            print("Unable to close all widgets!");
                        root.close();
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

    background: Rectangle {
        anchors.fill: parent
        radius: 4
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 6
        }
    }
}
