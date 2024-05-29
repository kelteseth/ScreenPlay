import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlayUtil
import "../ContentSettings"
import "../Components"

Drawer {
    id: root
    height: 250
    modal: false
    edge: Qt.BottomEdge
    background: Rectangle {
        color: Material.color(Material.Grey, Material.Shade900)
    }

    property bool hasPreviewGif: false
    property var type: ContentTypes.InstalledType.QMLWallpaper
    property string contentFolderName
    onClosed: {
        root.contentFolderName = ""
        root.type = ContentTypes.InstalledType.Unknown
    }

    function setInstalledDrawerItem(folderName, type) {

        // Toggle sidebar if clicked on the same content twice
        if (root.contentFolderName === folderName)
            return

        if (!App.util.isWallpaper(root.type)){
            return
        }

        root.contentFolderName = folderName
        root.type = type
        if (type === ContentTypes.InstalledType.VideoWallpaper)
            installedDrawerWrapper.state = "wallpaper"
        else
            installedDrawerWrapper.state = "scene"
        btnLaunchContent.text = qsTr("Set Wallpaper")
        root.open()
    }

    function indexOfValue(model, value) {
        for (var i = 0; i < model.length; i++) {
            let ourValue = model[i].value
            if (value === ourValue)
                return i
        }
        return -1
    }

    // This is used for removing wallpaper. We need to clear
    // the preview image/gif so we can release the file for deletion.
    function clear() {
        root.close()
        imagePreview.source = ""
        animatedImagePreview.source = ""
        txtHeadline.text = ""
        installedDrawerWrapper.state = "inactive"
    }

    onContentFolderNameChanged: {
        const item = App.installedListModel.get(root.contentFolderName)
        //txtHeadline.text = item.m_title
        const previewGiFilePath = Qt.resolvedUrl(
                                    item.m_absoluteStoragePath + "/" + item.m_previewGIF)
        const previewImageFilePath = Qt.resolvedUrl(
                                       item.m_absoluteStoragePath + "/" + item.m_preview)
        root.hasPreviewGif = App.util.fileExists(previewGiFilePath)
        if (hasPreviewGif) {
            animatedImagePreview.source = previewGiFilePath
            animatedImagePreview.playing = true
        } else {
            imagePreview.source = previewImageFilePath
        }
        if (App.util.isWidget(root.type)
                || (monitorSelection.activeMonitors.length > 0)) {
            btnLaunchContent.enabled = true
            return
        }
        btnLaunchContent.enabled = false
    }

    Item {
        anchors.fill: parent

        RowLayout {
            id: installedDrawerWrapper
            state: "inactive"
            spacing: 30

            anchors {
                margins: 10
                fill: parent
            }

            ColumnLayout {
                spacing: 5
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 3

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
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Connections {
                        target: App.screenPlayManager
                        function onPrintQmlTimeline() {
                            timeline.printTimelines()
                        }
                    }

                    ToolButton {
                        text: "❌" //qsTr("Remove all timeline ranges")
                        enabled: timeline.length > 1
                        onClicked: timeline.removeAll()
                        anchors {
                            right: parent.right
                            top: parent.top
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.horizontalStretchFactor: 2
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
                    height: 180
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    availableWidth: width
                    availableHeight: height - 20
                    fontSize: 11
                }
            }

            ColumnLayout {
                Layout.horizontalStretchFactor: 1
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    id: imageWrapper
                    color: "#2b2b2b"

                    Layout.preferredHeight: 100
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
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    objectName: "btnLaunchContent"
                    enabled: App.util.isWidget(root.type) && activeMonitors.length > 0 ? true : monitorSelection.isSelected
                    icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_plus.svg"
                    icon.color: "white"
                    font.pointSize: 12
                    onClicked: {
                        const item = App.installedListModel.get(
                                       root.contentFolderName)
                        const absoluteStoragePath = item.m_absoluteStoragePath
                        const previewImage = item.m_preview
                        if (App.util.isWallpaper(root.type)) {
                            if (type === ContentTypes.InstalledType.GodotWallpaper) {
                                if (App.globalVariables.isBasicVersion()) {
                                    installedDrawerWrapper.state = "inactive"
                                    return
                                }
                            }
                            let activeMonitors = monitorSelection.getActiveMonitors()
                            if (type === ContentTypes.InstalledType.GodotWallpaper) {
                                App.util.exportGodotProject(
                                            absoluteStoragePath,
                                            App.globalVariables.godotEditorExecutablePath).then(
                                            result => {
                                                if (!result.success) {
                                                    dialog.title = ("Error exporting Godot")
                                                    dialog.message = result.message
                                                    dialog.open()
                                                } else {
                                                    const screenFile = item.m_file
                                                     let volume = 1
                                                    let success = App.screenPlayManager.createWallpaper(
                                                        root.type,
                                                        cbVideoFillMode.currentValue,
                                                        absoluteStoragePath,
                                                        previewImage,
                                                        screenFile,
                                                        activeMonitors,
                                                        volume,
                                                        1, {}, true)
                                                }
                                            })
                                root.close()
                                return
                            }
                            const activeTimeline = timeline.getActiveTimeline()
                            const file = item.m_file
                            let success = App.screenPlayManager.setWallpaperAtTimelineIndex(
                                    root.type,
                                    absoluteStoragePath,
                                    previewImage,
                                    file,
                                    activeMonitors,
                                    timeline.activeTimelineIndex,
                                    activeTimeline.identifier, true)
                        }
                        root.close()
                        monitorSelection.reset()
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
                root.close()
                installedDrawerWrapper.state = "inactive"
            }
        }
    }
}
