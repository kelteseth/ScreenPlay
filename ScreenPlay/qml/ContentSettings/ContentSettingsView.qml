import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls.Material.impl
import ScreenPlay
import ScreenPlayCore as Util
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup
import "../Components"

Popup {
    id: root
    focus: true
    property string selectedSectionIdentifier
    property int selectedTimelineIndex
    property int selectedMonitorIndex
    property int selectedInstallType
    property Item modalSource
    property int maxWidth: 1200
    width: Math.min(Math.max(modalSource.width - 20, applicationWindow.minimumWidth), maxWidth)
    height: Math.min(Math.max(modalSource.height - 20, applicationWindow.minimumHeight), 800)

    onAboutToHide: {
        // Fixes the ugly transition with
        // our ModalBackgroundBlur on exit
        modal = false;
    }

    onAboutToShow: {
        modal = true;
        timeline.reset();
        monitorSelection.resize();
        monitorSelection.selectMonitorAt(0);
    }

    anchors.centerIn: root.modalSource

    modal: true

    Overlay.modal: Util.ModalBackgroundBlur {
        id: blurBg
        sourceItem: root.modalSource
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
        function onRequestToggleWallpaperConfiguration(): void {
            root.open();
        }

        target: App.uiAppStateSignals
    }
    ColumnLayout {

        anchors {
            fill: parent
            margins: 10
        }

        Rectangle {
            id: timelineWrapper
            color: Material.theme === Material.Light ? Material.backgroundColor : "#242424"
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
                    visible: !App.globalVariables.isBasicVersion()
                    modalSource: root.modalSource
                    onSelectedTimelineIndexChanged: {
                        const selectedTimeline = timeline.getSelectedTimeline();
                        print("onSelectedTimelineIndexChanged");
                        if (!selectedTimeline) {
                            console.error("Invalid selectedTimeline");
                            return;
                        }
                        root.selectedTimelineIndex = selectedTimeline.index;
                        root.selectedSectionIdentifier = selectedTimeline.identifier;
                        wallpaperControlsWrapper.updateControls();
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: App.globalVariables.isBasicVersion()

                    MultiEffect {
                        anchors.fill: parent
                        source: timeline
                        blurEnabled: true
                        blur: 0.8
                        blurMax: 32
                        brightness: -0.1
                    }

                    Column {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottom: parent.bottom
                            bottomMargin: 5
                        }
                        spacing: 10

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Material.secondaryTextColor
                            font.pointSize: 16
                            text: qsTr("Get Pro version to unlock timeline wallpaper")
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Button {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("Learn More")
                            onClicked: {
                                screenPlayProView.open();
                            }
                        }
                    }

                    ScreenPlayProPopup {
                        id: screenPlayProView
                        modalSource: root.modalSource
                    }
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
                    onSelected: function (index) {
                        App.monitorListModel.setSelectedIndex(index);
                    }

                    onDeselected: function () {
                        defaultVideoControls.visible = false;
                        customPropertiesGridView.visible = false;
                        root.selectedInstallType = Util.ContentTypes.InstalledType.Unknown;
                    }

                    onRequestProjectSettings: function (index, installedType, appID) {
                        console.log("Selected index:", index, "type:", installedType, "appID: ", appID);
                        if (root.selectedInstallType !== installedType) {
                            defaultVideoControls.visible = false;
                            customPropertiesGridView.visible = false;
                        }
                        root.selectedInstallType = installedType;

                        console.log(installedType, Util.ContentTypes.InstalledType.VideoWallpaper);
                        root.selectedMonitorIndex = index;
                        wallpaperControlsWrapper.updateControls();
                    }
                    onRequestRemoveWallpaper: index => {
                        const selectedTimeline = timeline.getSelectedTimeline();
                        if (selectedTimeline === undefined) {
                            console.error("No active timeline to remove wallpaper ", index);
                            return;
                        }
                        monitorSelection.enabled = false;
                        App.screenPlayManager.removeWallpaperAt(selectedTimeline.index, selectedTimeline.identifier, index).then(result => {
                            monitorSelection.enabled = true;
                            if (result.success) {
                                // Reset to update the wallpaper preview image
                                timeline.setActiveWallpaperPreviewImage();
                            } else {
                                InstantPopup.openErrorPopup(Window.window.contentItem, result.message);
                            }
                        });
                    }

                    anchors {
                        top: txtHeadline.bottom
                        topMargin: 20
                        left: parent.left
                        leftMargin: 20
                    }
                }

                RowLayout {
                    spacing: 5

                    anchors {
                        top: monitorSelection.bottom
                        right: parent.right
                        left: parent.left
                        margins: 20
                    }

                    Button {
                        id: btnRemoveAllWallpaper

                        text: {
                            const count = App.screenPlayManager.activeWallpaperCounter;
                            if (count === 0) {
                                qsTr("No active Wallpaper");
                            } else {
                                return qsTr("Remove all") + " " + App.screenPlayManager.activeWallpaperCounter + " " + qsTr("running Wallpaper");
                            }
                        }
                        Material.background: Material.accent
                        highlighted: true
                        font.family: App.settings.font
                        enabled: App.screenPlayManager.activeWallpaperCounter > 0
                        onClicked: {
                            App.screenPlayManager.removeAllRunningWallpapers(true).then(result => {
                                if (!result.success) {
                                    InstantPopup.openErrorPopup(timeline, result.message);
                                }
                            });
                        }
                    }

                    Button {
                        id: btnRemoveAllWidgets

                        text: {
                            const count = App.screenPlayManager.activeWidgetsCounter;
                            if (count === 0) {
                                qsTr("No active Widgets");
                            } else {
                                return qsTr("Remove all") + " " + App.screenPlayManager.activeWidgetsCounter + " " + qsTr("running Widgets");
                            }
                        }

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
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }

            Rectangle {
                id: wallpaperControlsWrapper
                color: Material.theme === Material.Light ? Material.backgroundColor : Qt.darker(Material.backgroundColor)
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

                function updateControls(): void {
                    print("updateControls");
                    if (root.selectedInstallType === Util.ContentTypes.InstalledType.VideoWallpaper) {
                        const selectedTimeline = timeline.getSelectedTimeline();
                        if (selectedTimeline === undefined) {
                            print("Invalid selected timeline");
                            defaultVideoControls.visible = false;
                            return;
                        }
                        customPropertiesGridView.visible = false;
                        defaultVideoControls.visible = true;
                        defaultVideoControls.state = "visible";
                        const wallpaperData = App.screenPlayManager.getWallpaperData(root.selectedMonitorIndex, selectedTimeline.index, selectedTimeline.identifier);
                        defaultVideoControls.wallpaperData = wallpaperData;
                        defaultVideoControls.monitorIndex = root.selectedMonitorIndex;
                        defaultVideoControls.timelineActive = selectedTimeline.lineIndicator.isActive;
                        defaultVideoControls.timelineIndex = selectedTimeline.index;
                        defaultVideoControls.sectionIdentifier = selectedTimeline.identifier;
                        return;
                    }
                    if (root.selectedInstallType === Util.ContentTypes.InstalledType.QMLWallpaper || root.selectedInstallType === Util.ContentTypes.InstalledType.GodotWallpaper || root.selectedInstallType === Util.ContentTypes.InstalledType.WebsiteWallpaper) {
                        let success = App.screenPlayManager.projectSettingsAtMonitorIndex(root.selectedMonitorIndex, root.selectedTimelineIndex, root.selectedSectionIdentifier);
                        if (!success) {
                            console.error("Unable to get requested settings from index: ", root.selectedTimelineIndex);
                            customPropertiesGridView.visible = false;
                            defaultVideoControls.visible = false;
                            return;
                        }
                        const selectedTimeline = timeline.getSelectedTimeline();
                        if (selectedTimeline === undefined) {
                            print("Invalid selected timeline");
                            return;
                        }
                        customPropertiesGridView.timelineActive = selectedTimeline.lineIndicator.isActive;
                        customPropertiesGridView.timelineIndex = selectedTimeline.index;
                        customPropertiesGridView.sectionIdentifier = selectedTimeline.identifier;
                        customPropertiesGridView.selectedMonitorIndex = root.selectedMonitorIndex;
                        customPropertiesGridView.projectSettingsListmodelRef = App.screenPlayManager.projectSettingsListModel;
                        console.log(customPropertiesGridView.timelineActive, customPropertiesGridView.timelineIndex, customPropertiesGridView.sectionIdentifier, customPropertiesGridView.selectedMonitorIndex, customPropertiesGridView.projectSettingsListmodelRef);
                        customPropertiesGridView.visible = true;
                        defaultVideoControls.visible = false;
                        return;
                    }
                }

                DefaultVideoControls {
                    id: defaultVideoControls
                    anchors.fill: parent
                    anchors.margins: 10
                    visible: false
                }

                GridView {
                    id: customPropertiesGridView
                    property var projectSettingsListmodelRef
                    property int selectedMonitorIndex
                    property bool timelineActive
                    property int timelineIndex
                    property string sectionIdentifier

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
                        height: 30
                        selectedMonitorIndex: root.selectedMonitorIndex
                        projectSettingsListmodelRef: customPropertiesGridView.projectSettingsListmodelRef
                        timelineActive: customPropertiesGridView.timelineActive
                        timelineIndex: customPropertiesGridView.timelineIndex
                        sectionIdentifier: customPropertiesGridView.sectionIdentifier
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
        icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/font-awsome/close.svg"
        icon.color: Material.iconColor
        onClicked: root.close()

        anchors {
            top: parent.top
            right: parent.right
        }
    }
}
