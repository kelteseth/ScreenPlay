import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlayUtil
import "../../../ScreenPlayUtil/qml/InstantPopup.js" as InstantPopup
import "../ContentSettings"
import "../Components"

Drawer {
    id: root
    height: 300
    modal: false
    edge: Qt.BottomEdge
    background: Rectangle {
        color: Material.color(Material.Grey, Material.Shade900)
    }
    onOpened: {
        timeline.reset();
    }

    property bool hasPreviewGif: false
    property var type: ContentTypes.InstalledType.QMLWallpaper
    property string contentFolderName

    function setInstalledDrawerItem(folderName, type) {

        // Toggle drawer if clicked on the same content twice
        if (root.contentFolderName === folderName) {
            if (root.visible) {
                root.close();
                return;
            }
        }
        if (!App.util.isWallpaper(type)) {
            return;
        }
        print("setInstalledDrawerItem", root.contentFolderName, folderName, typeof (folderName), type);
        root.contentFolderName = folderName;
        root.type = type;
        if (type === ContentTypes.InstalledType.VideoWallpaper)
            installedDrawerWrapper.state = "wallpaper";
        else
            installedDrawerWrapper.state = "scene";
        root.open();
    }

    // This is used for removing wallpaper. We need to clear
    // the preview image/gif so we can release the file for deletion.
    function clear() {
        root.close();
        root.contentFolderName = "";
        root.type = ContentTypes.InstalledType.Unknown;
        imagePreview.source = "";
        animatedImagePreview.source = "";
        txtHeadline.text = "";
        installedDrawerWrapper.state = "inactive";
    }

    onContentFolderNameChanged: {
        if (root.contentFolderName === "") {
            console.error("empty folder name");
            return;
        }
        const item = App.installedListModel.get(root.contentFolderName);
        print(root.contentFolderName);
        txtHeadline.text = item.m_title;
        const previewGiFilePath = Qt.resolvedUrl(item.m_absoluteStoragePath + "/" + item.m_previewGIF);
        const previewImageFilePath = Qt.resolvedUrl(item.m_absoluteStoragePath + "/" + item.m_preview);
        root.hasPreviewGif = App.util.fileExists(previewGiFilePath);
        if (hasPreviewGif) {
            animatedImagePreview.source = previewGiFilePath;
            animatedImagePreview.playing = true;
        } else {
            imagePreview.source = previewImageFilePath;
        }
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
                    text: qsTr("1. Select the duration your wallpaper should be visible")
                    font.family: App.settings.font
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 14
                    color: Material.secondaryTextColor
                }

                Timeline {
                    id: timeline
                    Layout.topMargin: 50
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            ColumnLayout {
                Layout.horizontalStretchFactor: 4
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                Text {
                    text: qsTr("2. Select a Monitor to display the content")
                    font.family: App.settings.font
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 14
                    color: Material.secondaryTextColor
                }

                MonitorSelection {
                    id: monitorSelection
                    objectName: "monitorSelection"
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
                                timeline.reset();
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
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    objectName: "btnLaunchContent"
                    text: qsTr("Set Wallpaper")
                    enabled: monitorSelection.isSelected && timeline.selectedTimelineIndex > -1
                    icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_plus.svg"
                    icon.color: "white"
                    font.pointSize: 12
                    onClicked: {
                        btnLaunchContent.enabled = false;
                        const item = App.installedListModel.get(root.contentFolderName);
                        const absoluteStoragePath = item.m_absoluteStoragePath;
                        const previewImage = item.m_preview;
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
                                        const file = item.m_file;
                                        let volume = 1;
                                        const selectedTimeline = timeline.getSelectedTimeline();
                                        App.screenPlayManager.setWallpaperAtTimelineIndex(root.type, absoluteStoragePath, previewImage, file, activeMonitors, selectedTimeline.index, selectedTimeline.identifier, true).then(result => {
                                            btnLaunchContent.enabled = true;
                                            if (!result.success) {
                                                InstantPopup.openErrorPopup(timeline, result.message);
                                                return;
                                            }
                                        });
                                    }
                                });
                                root.close();
                                return;
                            }
                            const selectedTimeline = timeline.getSelectedTimeline();
                            const file = item.m_file;
                            App.screenPlayManager.setWallpaperAtTimelineIndex(root.type, absoluteStoragePath, previewImage, file, activeMonitors, selectedTimeline.index, selectedTimeline.identifier, true).then(result => {
                                btnLaunchContent.enabled = true;
                                if (!result.success) {
                                    InstantPopup.openErrorPopup(timeline, result.message);
                                    return;
                                }
                            });
                        }
                        btnLaunchContent.enabled = true;
                        root.close();
                        // monitorSelection.reset()
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

            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_close.svg"
            icon.width: 15
            icon.height: 15
            onClicked: {
                root.close();
                installedDrawerWrapper.state = "inactive";
            }
        }
    }
}
