import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlayUtil
import "../Monitors"
import "../Components"

Drawer {
    id: root
    height: 500
    modal: false
    edge: Qt.BottomEdge
    property bool hasPreviewGif: false
    property var type: ContentTypes.InstalledType.QMLWallpaper
    property string contentFolderName
    onClosed: {
        root.contentFolderName = "";
        root.type = ContentTypes.InstalledType.Unknown;
    }

    function setInstalledDrawerItem(folderName, type) {

        // Toggle sidebar if clicked on the same content twice
        if (root.contentFolderName === folderName)
            return;
        root.contentFolderName = folderName;
        root.type = type;
        if (App.util.isWallpaper(root.type)) {
            if (type === ContentTypes.InstalledType.VideoWallpaper)
                installedDrawerWrapper.state = "wallpaper";
            else
                installedDrawerWrapper.state = "scene";
            btnLaunchContent.text = qsTr("Set Wallpaper");
        } else {
            installedDrawerWrapper.state = "widget";
            btnLaunchContent.text = qsTr("Set Widget");
        }
        root.open();
    }

    function indexOfValue(model, value) {
        for (var i = 0; i < model.length; i++) {
            let ourValue = model[i].value;
            if (value === ourValue)
                return i;
        }
        return -1;
    }

    // This is used for removing wallpaper. We need to clear
    // the preview image/gif so we can release the file for deletion.
    function clear() {
        root.close();
        imagePreview.source = "";
        animatedImagePreview.source = "";
        txtHeadline.text = "";
        installedDrawerWrapper.state = "inactive";
    }

    onContentFolderNameChanged: {
        const item = App.installedListModel.get(root.contentFolderName);
        //txtHeadline.text = item.m_title
        const previewGiFilePath = Qt.resolvedUrl(item.m_absoluteStoragePath + "/" + item.m_previewGIF);
        const previewImageFilePath = Qt.resolvedUrl(item.m_absoluteStoragePath + "/" + item.m_preview);
        root.hasPreviewGif = App.util.fileExists(previewGiFilePath);
        if (hasPreviewGif) {
            animatedImagePreview.source = previewGiFilePath;
            animatedImagePreview.playing = true;
        } else {
            imagePreview.source = previewImageFilePath;
        }
        if (App.util.isWidget(root.type) || (monitorSelection.activeMonitors.length > 0)) {
            btnLaunchContent.enabled = true;
            return;
        }
        btnLaunchContent.enabled = false;
    }

    RowLayout {
        id: installedDrawerWrapper
        state: "inactive"
        spacing: 20

        anchors.fill: parent

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 10
            spacing: 5

            ColumnLayout {
                spacing: 5
                Layout.fillWidth: true
                Layout.preferredHeight: 160

                Text {
                    Layout.leftMargin: 20
                    text: qsTr("Select the duration your wallpaper should be visible")
                    font.family: App.settings.font
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 11
                    color: Material.secondaryTextColor
                }

                Timeline {
                    id: timeline
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Connections {
                        target: App.screenPlayManager
                        function onPrintQmlTimeline() {
                            timeline.printTimelines();
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.horizontalStretchFactor: 1

                    Button {
                        text: qsTr("Remove all timeline ranges")
                        onClicked: timeline.removeAll()
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                Text {
                    text: qsTr("Select a Monitor to display the content")
                    font.family: App.settings.font
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 11
                    color: Material.secondaryTextColor
                }

                MonitorSelection {
                    id: monitorSelection
                    objectName: "monitorSelection"
                    height: 200
                    Layout.fillWidth: true
                    availableWidth: width
                    availableHeight: height
                    fontSize: 11
                }
            }
        }

        ColumnLayout {
            Layout.preferredWidth: 260
            Layout.fillHeight: true
            spacing: 10

            Rectangle {
                id: imageWrapper
                color: "#2b2b2b"

                Layout.preferredHeight: 180
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

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

            ColumnLayout {
                spacing: 20
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

                LabelSlider {
                    id: sliderVolume

                    Layout.fillWidth: true
                    headline: qsTr("Set Volume")

                    slider {
                        stepSize: 0.01
                        from: 0
                        value: 1
                        to: 1
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5

                    Text {
                        id: txtComboBoxFillMode

                        visible: false
                        text: qsTr("Fill Mode")
                        font.family: App.settings.font
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: 10
                        color: "#626262"
                        wrapMode: Text.WrapAnywhere
                        Layout.fillWidth: true
                    }

                    ComboBox {
                        id: cbVideoFillMode

                        visible: false
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        font.family: App.settings.font
                        model: [
                            {
                                "value": Settings.FillMode.Stretch,
                                "text": qsTr("Stretch")
                            },
                            {
                                "value": Settings.FillMode.Fill,
                                "text": qsTr("Fill")
                            },
                            {
                                "value": Settings.FillMode.Contain,
                                "text": qsTr("Contain")
                            },
                            {
                                "value": Settings.FillMode.Cover,
                                "text": qsTr("Cover")
                            },
                            {
                                "value": Settings.FillMode.Scale_Down,
                                "text": qsTr("Scale-Down")
                            }
                        ]
                        Component.onCompleted: {
                            cbVideoFillMode.currentIndex = root.indexOfValue(cbVideoFillMode.model, App.settings.videoFillMode);
                        }
                    }
                }
            }

            Dialog {
                id: dialog
                standardButtons: Dialog.Ok
                title: qsTr("Export Godot project")
                property alias message: messageText.text
                Text {
                    id: messageText
                }
            }

            Button {
                id: btnLaunchContent
                objectName: "btnLaunchContent"
                enabled: App.util.isWidget(root.type) ? true : monitorSelection.isSelected
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_plus.svg"
                icon.color: "white"
                font.pointSize: 12
                onClicked: {
                    const item = App.installedListModel.get(root.contentFolderName);
                    const absoluteStoragePath = item.m_absoluteStoragePath;
                    const previewImage = item.m_preview;
                    if (App.util.isWallpaper(root.type)) {
                        if (type === ContentTypes.InstalledType.GodotWallpaper) {
                            if (App.globalVariables.isBasicVersion()) {
                                installedDrawerWrapper.state = "inactive";
                                return;
                            }
                        }
                        let activeMonitors = monitorSelection.getActiveMonitors();
                        // TODO Alert user to choose a monitor
                        if (activeMonitors.length === 0)
                            return;

                        // We only have sliderVolume if it is a VideoWallpaper
                        let volume = 0;
                        if (type === ContentTypes.InstalledType.VideoWallpaper)
                            volume = Math.round(sliderVolume.slider.value * 100) / 100;
                        if (type === ContentTypes.InstalledType.GodotWallpaper) {
                            App.util.exportGodotProject(absoluteStoragePath, App.globalVariables.godotEditorExecutablePath).then(result => {
                                if (!result.success) {
                                    dialog.title = ("Error exporting Godot");
                                    dialog.message = result.message;
                                    dialog.open();
                                } else {
                                    const screenFile = item.m_file;
                                    let success = App.screenPlayManager.createWallpaper(root.type, cbVideoFillMode.currentValue, absoluteStoragePath, previewImage, screenFile, activeMonitors, volume, 1, {}, true);
                                }
                            });
                            root.close();
                            return;
                        }
                        const activeTimeline = timeline.getActiveTimeline();
                        const screenFile = item.m_file;
                        const playbackRate = 1;
                        const jsonProperties = {};
                        let success = App.screenPlayManager.setWallpaperAtTimelineIndex(
                                root.type, cbVideoFillMode.currentValue, absoluteStoragePath, previewImage, screenFile, activeMonitors, volume, playbackRate, jsonProperties, timeline.activeTimelineIndex, activeTimeline.identifier, true);
                    }
                    if (App.util.isWidget(root.type))
                        App.screenPlayManager.startWidget(type, Qt.point(0, 0), absoluteStoragePath, previewImage, {}, true);
                    root.close();
                    monitorSelection.reset();
                }

                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            }
        }

        ToolButton {
            id: button
            Layout.alignment: Qt.AlignTop
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_close.svg"
            icon.width: 15
            icon.height: 15
            onClicked: {
                root.close();
                installedDrawerWrapper.state = "inactive";
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
                    target: sliderVolume
                    visible: false
                }

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

                PropertyChanges {
                    target: txtComboBoxFillMode
                    opacity: 1
                    visible: true
                }

                PropertyChanges {
                    target: cbVideoFillMode
                    opacity: 1
                    visible: true
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

                PropertyChanges {
                    target: sliderVolume
                    opacity: 0
                    visible: false
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
}
