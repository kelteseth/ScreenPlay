import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Enums.FillMode
import ScreenPlay.Enums.InstalledType
import "../Monitors"
import ScreenPlayUtil as Util

Item {
    id: root

    property real navHeight
    property var type: InstalledType.QMLWallpaper
    property string contentFolderName

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
        imagePreview.source = "";
        animatedImagePreview.source = "";
        txtHeadline.text = "";
        root.state = "inactive";
    }

    width: 400
    state: "inactive"
    property bool hasPreviewGif: false
    onContentFolderNameChanged: {
        txtHeadline.text = App.installedListModel.get(root.contentFolderName).m_title;
        const previewGiFilePath = Qt.resolvedUrl(App.globalVariables.localStoragePath + "/" + root.contentFolderName + "/" + App.installedListModel.get(root.contentFolderName).m_previewGIF);
        const previewImageFilePath = Qt.resolvedUrl(App.globalVariables.localStoragePath + "/" + root.contentFolderName + "/" + App.installedListModel.get(root.contentFolderName).m_preview);
        root.hasPreviewGif = App.util.fileExists(previewGiFilePath);
        if (hasPreviewGif) {
            animatedImagePreview.source = previewGiFilePath;
            animatedImagePreview.playing = true;
        } else {
            imagePreview.source = previewImageFilePath;
        }
        if (JSUtil.isWidget(root.type) || (monitorSelection.activeMonitors.length > 0)) {
            btnSetWallpaper.enabled = true;
            return;
        }
        btnSetWallpaper.enabled = false;
    }

    Connections {
        function onSetSidebarItem(folderName, type) {

            // Toggle sidebar if clicked on the same content twice
            if (root.contentFolderName === folderName && root.state !== "inactive") {
                root.state = "inactive";
                return;
            }
            root.contentFolderName = folderName;
            root.type = type;
            if (JSUtil.isWallpaper(root.type)) {
                if (type === InstalledType.VideoWallpaper)
                    root.state = "activeWallpaper";
                else
                    root.state = "activeScene";
                btnSetWallpaper.text = qsTr("Set Wallpaper");
            } else {
                root.state = "activeWidget";
                btnSetWallpaper.text = qsTr("Set Widget");
            }
        }

        target: App.util
    }

    Util.MouseHoverBlocker {
    }

    Rectangle {
        anchors.fill: parent
        color: Material.theme === Material.Light ? "white" : Material.background
        opacity: 0.95
        layer.enabled: true

        layer.effect: ElevationEffect {
            elevation: 4
        }
    }

    Item {
        id: sidebarWrapper

        anchors.fill: parent

        Item {
            id: navBackground

            height: navHeight

            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            Rectangle {
                anchors.fill: parent

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "transparent"
                    }
                    GradientStop {
                        position: 0.1
                        color: "#AAffffff"
                    }

                    GradientStop {
                        position: 1
                        color: "#ffffff"
                    }
                }
            }
        }

        Item {
            id: sidebarBackground

            anchors {
                top: navBackground.bottom
                right: parent.right
                bottom: parent.bottom
                left: parent.left
            }

            Rectangle {
                id: imageWrapper

                height: 237
                color: "#2b2b2b"
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0

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

                MouseArea {
                    id: button

                    height: 50
                    width: 50
                    anchors.top: parent.top
                    anchors.left: parent.left
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.state = "inactive"

                    Image {
                        id: imgBack

                        source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_arrow_right.svg"
                        sourceSize: Qt.size(15, 15)
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                    }
                }
            }

            ColumnLayout {
                spacing: 20

                anchors {
                    top: imageWrapper.bottom
                    right: parent.right
                    left: parent.left
                    margins: 30
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    Text {
                        id: txtHeadlineMonitor

                        height: 20
                        text: qsTr("Select a Monitor to display the content")
                        font.family: App.settings.font
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: 11
                        color: Material.secondaryTextColor
                    }

                    MonitorSelection {
                        id: monitorSelection
                        objectName: "monitorSelection"
                        height: 180
                        Layout.fillWidth: true
                        availableWidth: width
                        availableHeight: height
                        fontSize: 11
                    }
                }

                Util.Slider {
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
                        model: [{
                                "value": FillMode.Stretch,
                                "text": qsTr("Stretch")
                            }, {
                                "value": FillMode.Fill,
                                "text": qsTr("Fill")
                            }, {
                                "value": FillMode.Contain,
                                "text": qsTr("Contain")
                            }, {
                                "value": FillMode.Cover,
                                "text": qsTr("Cover")
                            }, {
                                "value": FillMode.Scale_Down,
                                "text": qsTr("Scale-Down")
                            }]
                        Component.onCompleted: {
                            cbVideoFillMode.currentIndex = root.indexOfValue(cbVideoFillMode.model, App.settings.videoFillMode);
                        }
                    }
                }
            }

            Button {
                id: btnSetWallpaper
                objectName: "btnSetWallpaper"
                enabled: JSUtil.isWidget(root.type) ? true : monitorSelection.isSelected
                Material.background: Material.accent
                Material.foreground: "white"
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_plus.svg"
                icon.color: "white"
                font.pointSize: 12
                onClicked: {
                    const absoluteStoragePath = App.globalVariables.localStoragePath + "/" + root.contentFolderName;
                    const previewImage = App.installedListModel.get(root.contentFolderName).m_preview;
                    if (JSUtil.isWallpaper(root.type)) {
                        let activeMonitors = monitorSelection.getActiveMonitors();
                        // TODO Alert user to choose a monitor
                        if (activeMonitors.length === 0)
                            return;

                        // We only have sliderVolume if it is a VideoWallpaper
                        let volume = 0;
                        if (type === InstalledType.VideoWallpaper)
                            volume = Math.round(sliderVolume.slider.value * 100) / 100;
                        const screenFile = App.installedListModel.get(root.contentFolderName).m_file;
                        let success = App.screenPlayManager.createWallpaper(root.type, cbVideoFillMode.currentValue, absoluteStoragePath, previewImage, screenFile, activeMonitors, volume, 1, {}, true);
                    }
                    if (JSUtil.isWidget(root.type))
                        App.screenPlayManager.createWidget(type, Qt.point(0, 0), absoluteStoragePath, previewImage, {}, true);
                    root.state = "inactive";
                    monitorSelection.reset();
                }

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 20
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    states: [
        State {
            name: "inactive"

            PropertyChanges {
                target: root
                anchors.rightMargin: -root.width
            }

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

            PropertyChanges {
                target: txtHeadlineMonitor
                opacity: 0
            }
        },
        State {
            name: "activeWallpaper"

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
                target: txtHeadlineMonitor
                opacity: 1
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
            name: "activeScene"

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
                target: txtHeadlineMonitor
                opacity: 1
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

            NumberAnimation {
                target: root
                properties: "anchors.rightMargin"
                duration: 250
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            to: "activeWidget"
            from: "*"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    properties: "anchors.rightMargin"
                    duration: 250
                    easing.type: Easing.OutQuart
                }

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
            to: "activeWallpaper"
            from: "*"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    properties: "anchors.rightMargin"
                    duration: 250
                    easing.type: Easing.OutQuart
                }

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
            to: "activeScene"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    properties: "anchors.rightMargin"
                    duration: 250
                    easing.type: Easing.OutQuart
                }

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
