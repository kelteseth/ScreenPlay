import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlay
import ScreenPlayCore
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup
import "../ContentSettings"
import "../Components"

Drawer {
    id: root
    height: 300
    modal: false
    closePolicy: Popup.NoAutoClose
    edge: Qt.BottomEdge
    background: Rectangle {
        color: Material.theme === Material.Dark ? Material.color(Material.Grey, Material.Shade900) : Material.color(Material.Grey, Material.Shade100)
    }
    onAboutToShow: {
        print("setInstalledDrawerItem onAboutToShow");
        timeline.reset();
        monitorSelection.resize();
        monitorSelection.selectOnly(0);
        btnLaunchContent.updateButtonEnabledState(0);
    }
    property bool hasPreviewGif: false
    property var type: ContentTypes.InstalledType.QMLWallpaper
    property string contentFolderName
    property Item modalSource

    function setInstalledDrawerItem(folderName: string, type: int): void {

        // Toggle drawer if clicked on the same content twice
        if (root.contentFolderName === folderName) {
            if (root.visible) {
                root.close();
                return;
            }
        }
        if (!App.util.isWallpaper(type)) {
            if (root.visible) {
                root.close();
                return;
            }
            return;
        }
        print("setInstalledDrawerItem", root.contentFolderName, folderName, typeof (folderName), type, root.visible);
        root.contentFolderName = folderName;
        root.type = type;
        const item = App.installedListModel.get(root.contentFolderName);
        print(root.contentFolderName);
        txtHeadline.text = item.title;
        const previewGiFilePath = Qt.resolvedUrl(item.absoluteStoragePath + "/" + item.previewGIF);
        const previewImageFilePath = Qt.resolvedUrl(item.absoluteStoragePath + "/" + item.preview);
        root.hasPreviewGif = App.util.fileExists(previewGiFilePath);
        if (hasPreviewGif) {
            animatedImagePreview.source = previewGiFilePath;
            animatedImagePreview.playing = true;
        } else {
            imagePreview.source = previewImageFilePath;
        }
        if (type === ContentTypes.InstalledType.VideoWallpaper)
            installedDrawerWrapper.state = "wallpaper";
        else
            installedDrawerWrapper.state = "scene";
        if (!root.visible) {
            console.log("setInstalledDrawerItem");
            root.open();
        }
    }

    // This is used for removing wallpaper. We need to clear
    // the preview image/gif so we can release the file for deletion.
    function clear(): void {
        console.warn("⚠️⚠️⚠️ CLEAR InstalledDrawer");
        root.close();
        root.contentFolderName = "";
        root.type = ContentTypes.InstalledType.Unknown;
        imagePreview.source = "";
        animatedImagePreview.source = "";
        txtHeadline.text = "";
        installedDrawerWrapper.state = "inactive";
    }

    Connections {
        function onRequestHideInstalledDrawer() {
            close();
        }

        target: App.uiAppStateSignals
    }

    Item {
        anchors.fill: parent

        RowLayout {
            id: installedDrawerWrapper
            state: "inactive"
            spacing: 20

            anchors {
                margins: 10
                fill: parent
            }

            ColumnLayout {
                spacing: 5
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 4

                Text {
                    Layout.leftMargin: 20
                    text: {
                        if (App.globalVariables.isBasicVersion()) {
                            qsTr("🚀You need ScreenPlay Pro (or compile it yourself)");
                        } else {
                            qsTr("1. Set the duration your wallpaper should be visible");
                        }
                    }

                    font.family: App.settings.font
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 14
                    color: Material.secondaryTextColor
                }
                Item {

                    Layout.topMargin: 10
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Timeline {
                        id: timeline
                        modalSource: root.modalSource
                        anchors.fill: parent
                        visible: !App.globalVariables.isBasicVersion()
                    }

                    Item {
                        anchors.fill: parent
                        visible: App.globalVariables.isBasicVersion()

                        MultiEffect {
                            anchors.fill: parent
                            source: timeline
                            blurEnabled: true
                            blur: 0.8
                            blurMax: 32
                            brightness: -0.1
                            visible: App.globalVariables.isBasicVersion()
                        }

                        Column {
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                bottom: parent.bottom
                                bottomMargin: 5
                            }

                            spacing: 10
                            visible: App.globalVariables.isBasicVersion()

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

            ColumnLayout {
                Layout.horizontalStretchFactor: 4
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                Text {
                    text: {
                        if (App.globalVariables.isBasicVersion()) {
                            qsTr("1. Set a Monitor to display the content");
                        } else {
                            qsTr("2. Set a Monitor to display the content");
                        }
                    }
                    font.family: App.settings.font
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 14
                    color: Material.secondaryTextColor
                }

                MonitorSelection {
                    id: monitorSelection
                    objectName: "monitorSelection"
                    onSelected: function (index) {
                        App.monitorListModel.setSelectedIndex(index);
                    }

                    onRequestRemoveWallpaper: monitorIndex => {
                        const selectedTimeline = timeline.getSelectedTimeline();
                        if (selectedTimeline === undefined) {
                            InstantPopup.openErrorPopup(timeline, qsTr("No active timeline to remove wallpaper"));
                            return;
                        }
                        monitorSelection.enabled = false;
                        App.screenPlayManager.removeWallpaperAt(selectedTimeline.index, selectedTimeline.identifier, monitorIndex).then(result => {
                            monitorSelection.enabled = true;
                            if (result.success) {
                                // Reset to update the wallpaper preview image
                                timeline.setActiveWallpaperPreviewImage();
                            } else {
                                InstantPopup.openErrorPopup(timeline, result.message);
                            }
                        });
                    }

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    fontSize: 11
                }
            }

            ColumnLayout {
                Layout.horizontalStretchFactor: 3
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    id: imageWrapper
                    color: "#2b2b2b"

                    Layout.preferredHeight: 160
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    Layout.topMargin: 20

                    // Do NOT enable async image loading!
                    // Otherwhise it will still hold the file
                    // when calling InstalledListModel::deinstallItemAt
                    // -> asynchronous: false
                    AnimatedImage {
                        id: animatedImagePreview
                        asynchronous: false
                        playing: true
                        fillMode: Image.PreserveAspectCrop
                        anchors.fill: parent
                        visible: enabled
                        enabled: root.hasPreviewGif
                    }

                    Image {
                        id: imagePreview
                        asynchronous: false
                        fillMode: Image.PreserveAspectCrop
                        anchors.fill: parent
                        enabled: !root.hasPreviewGif
                        visible: enabled
                    }

                    Rectangle {
                        id: tabShadow
                        height: 70

                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                            left: parent.left
                        }

                        gradient: Gradient {
                            GradientStop {
                                position: 1
                                color: "#EE000000"
                            }

                            GradientStop {
                                position: 0
                                color: "#00000000"
                            }
                        }
                    }

                    Text {
                        id: txtHeadline

                        text: qsTr("Headline")
                        font.family: App.settings.font
                        verticalAlignment: Text.AlignBottom
                        font.pointSize: 16
                        color: "white"
                        wrapMode: Text.WordWrap
                        height: 50

                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                            margins: 20
                            left: parent.left
                        }
                    }
                }

                Button {
                    id: btnLaunchContent
                    Layout.preferredWidth: 250
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    objectName: "btnLaunchContent"

                    text: {
                        // TODO check if it is active timline
                        if (App.globalVariables.isBasicVersion()) {
                            return qsTr("2. Start Wallpaper now");
                        } else {
                            if (App.screenPlayManager.selectedTimelineIndex == App.screenPlayManager.activeTimelineIndex) {
                                return qsTr("3. Start Wallpaper now");
                            } else {
                                return qsTr("3. Set Wallpaper");
                            }
                        }
                    }
                    Connections {
                        target: App.monitorListModel
                        // Disable start button while busy
                        function onSelectedIndexChanged(index) {
                            btnLaunchContent.updateButtonEnabledState();
                        }
                        function onDataChanged(topLeft, bottomRight, roles) {
                            if (roles.indexOf(MonitorListModel.MonitorRole.AppState) !== -1) {
                                btnLaunchContent.updateButtonEnabledState();
                            }
                        }
                    }
                    function updateButtonEnabledState() {
                        const selectedAndValid = monitorSelection.isSelected && timeline.selectedTimelineIndex > -1;

                        // Create a proper model index using the index() method
                        const modelIndex = App.monitorListModel.index(App.monitorListModel.selectedIndex, 0);

                        // Now use this model index with the data() method
                        const state = App.monitorListModel.data(modelIndex, MonitorListModel.MonitorRole.AppState);
                        print("onSelectedIndexChanged state: ", state, btnLaunchContent.enabled);
                        if (state == ScreenPlayEnums.AppState.NotSet || state == ScreenPlayEnums.AppState.Active) {
                            btnLaunchContent.enabled = selectedAndValid;
                            return;
                        }
                        btnLaunchContent.enabled = false;
                    }

                    icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_plus.svg"
                    icon.color: "white"
                    font.pointSize: 12
                    onClicked: {
                        // Close only the basic version, where the user only can select
                        // one wallpaper anyway...
                        const isBasicVersion = App.globalVariables.isBasicVersion();
                        btnLaunchContent.enabled = false;
                        const item = App.installedListModel.get(root.contentFolderName);
                        const absoluteStoragePath = item.absoluteStoragePath;
                        const previewImage = item.preview;
                        const title = item.title;
                        if (App.util.isWallpaper(root.type)) {
                            if (type === ContentTypes.InstalledType.GodotWallpaper) {
                                if (App.globalVariables.isBasicVersion()) {
                                    InstantPopup.openErrorPopup(timeline, qsTr("You are not allowed to do that!"));
                                    installedDrawerWrapper.state = "inactive";
                                    return;
                                }
                            }
                            let activeMonitors = monitorSelection.getActiveMonitors();
                            if (type === ContentTypes.InstalledType.GodotWallpaper) {
                                App.util.exportGodotProject(absoluteStoragePath, App.globalVariables.godotEditorExecutablePath).then(result => {
                                    if (!result.success) {
                                        btnLaunchContent.enabled = true;
                                        InstantPopup.openErrorPopup(timeline, result.message);
                                    } else {
                                        const file = item.file;
                                        const selectedTimeline = timeline.getSelectedTimeline();
                                        const saveToProfilesConfigFile = true;
                                        App.screenPlayManager.setWallpaperAtMonitorTimelineIndex(root.type, absoluteStoragePath, previewImage, file, title, activeMonitors, selectedTimeline.index, selectedTimeline.identifier, saveToProfilesConfigFile).then(result => {
                                            btnLaunchContent.enabled = true;
                                            if (!result.success) {
                                                InstantPopup.openErrorPopup(timeline, result.message);
                                                return;
                                            }
                                        });
                                    }
                                });
                                if (isBasicVersion) {
                                    root.close();
                                }
                                return;
                            }
                            const selectedTimeline = timeline.getSelectedTimeline();
                            const file = item.file;
                            App.screenPlayManager.setWallpaperAtMonitorTimelineIndex(root.type, absoluteStoragePath, previewImage, file, title, activeMonitors, selectedTimeline.index, selectedTimeline.identifier, true).then(result => {
                                btnLaunchContent.enabled = true;
                                if (!result.success) {
                                    InstantPopup.openErrorPopup(timeline, result.message);
                                    return;
                                }
                            });
                        }
                        btnLaunchContent.enabled = true;
                        if (isBasicVersion) {
                            root.close();
                        }
                    }
                }
            }

            states: [
                State {
                    name: "inactive"

                    PropertyChanges {
                        target: imagePreview
                        opacity: 0
                        anchors.topMargin: 20
                    }
                    PropertyChanges {
                        target: animatedImagePreview
                        opacity: 0
                        anchors.topMargin: 20
                    }
                },
                State {
                    name: "activeWidget"

                    PropertyChanges {
                        target: monitorSelection
                        visible: false
                        enabled: false
                    }

                    PropertyChanges {
                        target: imagePreview
                        opacity: 1
                        anchors.topMargin: 0
                    }
                    PropertyChanges {
                        target: animatedImagePreview
                        opacity: 1
                        anchors.topMargin: 0
                    }
                },
                State {
                    name: "wallpaper"

                    PropertyChanges {
                        target: imagePreview
                        opacity: 1
                        anchors.topMargin: 0
                    }
                    PropertyChanges {
                        target: animatedImagePreview
                        opacity: 1
                        anchors.topMargin: 0
                    }
                },
                State {
                    name: "scene"

                    PropertyChanges {
                        target: imagePreview
                        opacity: 1
                        anchors.topMargin: 0
                    }
                    PropertyChanges {
                        target: animatedImagePreview
                        opacity: 1
                        anchors.topMargin: 0
                    }
                }
            ]
            transitions: [
                Transition {
                    to: "inactive"
                    from: "*"
                    reversible: true

                    NumberAnimation {
                        targets: [animatedImagePreview, imagePreview]
                        property: "opacity"
                        duration: 200
                    }

                    NumberAnimation {
                        targets: [animatedImagePreview, imagePreview]
                        property: "anchors.topMargin"
                        duration: 400
                    }
                },
                Transition {
                    to: "widget"
                    from: "*"

                    SequentialAnimation {
                        ParallelAnimation {
                            NumberAnimation {
                                targets: [animatedImagePreview, imagePreview]
                                property: "opacity"
                                duration: 200
                            }

                            NumberAnimation {
                                targets: [animatedImagePreview, imagePreview]
                                property: "anchors.topMargin"
                                duration: 100
                            }
                        }
                    }
                },
                Transition {
                    to: "wallpaper"
                    from: "*"

                    SequentialAnimation {
                        ParallelAnimation {
                            NumberAnimation {
                                targets: [animatedImagePreview, imagePreview]
                                property: "opacity"
                                duration: 200
                            }

                            NumberAnimation {
                                targets: [animatedImagePreview, imagePreview]
                                property: "anchors.topMargin"
                                duration: 100
                            }
                        }
                    }
                },
                Transition {
                    to: "scene"

                    SequentialAnimation {
                        ParallelAnimation {
                            NumberAnimation {
                                targets: [animatedImagePreview, imagePreview]
                                property: "opacity"
                                duration: 200
                            }

                            NumberAnimation {
                                targets: [animatedImagePreview, imagePreview]
                                property: "anchors.topMargin"
                                duration: 100
                            }
                        }
                    }
                }
            ]
        }

        ToolButton {
            id: button
            anchors {
                top: parent.top
                topMargin: -20
                right: parent.right
            }

            icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_close.svg"
            icon.width: 15
            icon.height: 15
            onClicked: {
                root.close();
                installedDrawerWrapper.state = "inactive";
            }
        }
    }
}
