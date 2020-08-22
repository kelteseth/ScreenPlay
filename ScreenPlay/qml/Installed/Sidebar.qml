import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import ScreenPlay 1.0
import ScreenPlay.Enums.FillMode 1.0
import ScreenPlay.Enums.InstalledType 1.0

import "../Monitors"
import "../Common" as SP

Item {
    id: root
    width: 400
    state: "inactive"

    property real navHeight
    property var type: InstalledType.QMLWallpaper
    property string contentFolderName

    onContentFolderNameChanged: {
        txtHeadline.text = ScreenPlay.installedListModel.get(
                    root.contentFolderName).screenTitle

        const hasPreviewGif = ScreenPlay.installedListModel.get(
                                root.contentFolderName).screenPreviewGIF !== undefined
        if (!hasPreviewGif) {
            image.source = Qt.resolvedUrl(
                        ScreenPlay.globalVariables.localStoragePath + "/"
                        + root.contentFolderName + "/" + ScreenPlay.installedListModel.get(
                            root.contentFolderName).screenPreview)
            image.playing = false
        } else {

            image.source = Qt.resolvedUrl(
                        ScreenPlay.globalVariables.localStoragePath + "/"
                        + root.contentFolderName + "/" + ScreenPlay.installedListModel.get(
                            root.contentFolderName).screenPreviewGIF)
            image.playing = true
        }

        if (isWidget() || (monitorSelection.activeMonitors.length > 0)) {
            btnSetWallpaper.enabled = true
            return
        }

        btnSetWallpaper.enabled = false
    }

    function isWallpaper() {
        return type === InstalledType.VideoWallpaper
                || type === InstalledType.HTMLWallpaper
                || type === InstalledType.QMLWallpaper
    }

    function isWidget() {
        return type === InstalledType.HTMLWidget
                || type === InstalledType.QMLWidget
    }

    function indexOfValue(model, value) {

        for (var i = 0; i < model.length; i++) {
            let ourValue = model[i].value
            if (value === ourValue)
                return i
        }
        return -1
    }

    Connections {
        target: ScreenPlay.util

        function onSetSidebarItem(folderName, type) {

            // Toggle sidebar if clicked on the same content twice
            if (root.contentFolderName === folderName
                    && root.state !== "inactive") {
                root.state = "inactive"
                return
            }

            root.contentFolderName = folderName
            root.type = type

            if (root.isWallpaper()) {
                if (type === InstalledType.VideoWallpaper) {
                    root.state = "activeWallpaper"
                } else {
                    root.state = "activeScene"
                }
                btnSetWallpaper.text = qsTr("Set Wallpaper")
            } else {
                root.state = "activeWidget"
                btnSetWallpaper.text = qsTr("Set Widget")
            }
        }
    }

    Item {
        id: sidebarWrapper
        anchors.fill: parent
        MouseArea {
            id: mouseAreaHelper
            anchors.fill: parent
            enabled: true
            hoverEnabled: true
            propagateComposedEvents: false
        }
        Item {
            id: navBackground
            height: navHeight
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            LinearGradient {
                anchors.fill: parent
                start: Qt.point(0, 0)
                end: Qt.point(400, 0)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "transparent"
                    }
                    GradientStop {
                        position: 0.1
                        color: "#AAffffff"
                    }
                    GradientStop {
                        position: 1.0
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
                leftMargin: 5
            }

            Rectangle {
                anchors.fill: parent
                color: Material.theme === Material.Light ? "white" : Material.background
                opacity: 0.95
            }

            Rectangle {
                id: imageWrapper
                height: 237
                color: "#2b2b2b"
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0

                AnimatedImage {
                    id: image
                    playing: true
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    anchors.fill: parent
                    onStatusChanged: {
                        if (image.status === Image.Error) {
                            source = "qrc:/assets/images/missingPreview.png"
                        }
                    }
                }
                LinearGradient {
                    id: tabShadow
                    height: 50
                    cached: true

                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        left: parent.left
                    }
                    start: Qt.point(0, 50)
                    end: Qt.point(0, 0)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "#EE000000"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#00000000"
                        }
                    }
                }

                Text {
                    id: txtHeadline
                    text: qsTr("Headline")
                    font.family: ScreenPlay.settings.font
                    font.weight: Font.Thin
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
                        source: "qrc:/assets/icons/icon_arrow_right.svg"
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
                        font.family: ScreenPlay.settings.font
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: 10
                        color: "#626262"
                    }

                    MonitorSelection {
                        id: monitorSelection
                        height: 180
                        Layout.fillWidth: true
                        availableWidth: width
                        availableHeight: height
                        fontSize: 11
                        onActiveMonitorsChanged: {
                            if (isWidget()) {
                                btnSetWallpaper.enabled = true
                                return
                            }

                            btnSetWallpaper.enabled = activeMonitors.length > 0
                        }
                    }
                }

                SP.Slider {
                    id: sliderVolume
                    slider {
                        stepSize: 0.01
                        from: 0
                        value: 1
                        to: 1
                    }

                    Layout.fillWidth: true
                    headline: qsTr("Set Volume")
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5

                    Text {
                        id: txtComboBoxFillMode
                        visible: false
                        text: qsTr("Fill Mode")

                        font.family: ScreenPlay.settings.font
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
                        font.family: ScreenPlay.settings.font
                        Component.onCompleted: {
                            cbVideoFillMode.currentIndex = root.indexOfValue(
                                        cbVideoFillMode.model,
                                        ScreenPlay.settings.videoFillMode)
                        }
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
                    }
                }
            }

            Button {
                id: btnSetWallpaper
                Material.background: Material.accent
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                icon.source: "qrc:/assets/icons/icon_plus.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 20
                    horizontalCenter: parent.horizontalCenter
                }

                onClicked: {
                    const absoluteStoragePath = ScreenPlay.globalVariables.localStoragePath
                                              + "/" + root.contentFolderName
                    const previewImage = ScreenPlay.installedListModel.get(
                                           root.contentFolderName).screenPreview
                    if (root.isWallpaper()) {
                        let activeMonitors = monitorSelection.getActiveMonitors(
                                )

                        // TODO Alert user to choose a monitor
                        if (activeMonitors.length === 0)
                            return

                        // We only have sliderVolume if it is a VideoWallpaper
                        let volume = 0.0
                        if (type === InstalledType.VideoWallpaper) {
                            volume = Math.round(sliderVolume.value * 100) / 100
                        }

                        const screenFile = ScreenPlay.installedListModel.get(
                                             root.contentFolderName).screenFile

                        ScreenPlay.screenPlayManager.createWallpaper(
                                    root.type, cbVideoFillMode.currentValue,
                                    absoluteStoragePath, previewImage,
                                    screenFile, activeMonitors, volume,
                                    1.0, {}, true)
                    }

                    if (root.isWidget()) {
                        ScreenPlay.screenPlayManager.createWidget(
                                    type, Qt.point(0, 0), absoluteStoragePath,
                                    previewImage, {}, true)
                    }

                    root.state = "inactive"
                    monitorSelection.reset()
                }
            }
        }

        Item {
            id: shadow
            width: 5

            anchors {
                top: parent.top
                right: sidebarBackground.left
                bottom: parent.bottom
                left: parent.left
            }

            LinearGradient {
                anchors.fill: parent
                start: Qt.point(0, 0)
                end: Qt.point(5, 0)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "#00000000"
                    }
                    GradientStop {
                        position: 1.0
                        color: "#22000000"
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "inactive"

            PropertyChanges {
                target: mouseAreaHelper
                enabled: false
            }

            PropertyChanges {
                target: root
                anchors.rightMargin: -root.width
            }
            PropertyChanges {
                target: image
                opacity: 0
                anchors.topMargin: 20
            }
        },
        State {
            name: "activeWidget"
            PropertyChanges {
                target: mouseAreaHelper
                enabled: true
            }

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
                target: image
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
                target: mouseAreaHelper
                enabled: true
            }

            PropertyChanges {
                target: image
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
                target: mouseAreaHelper
                enabled: true
            }

            PropertyChanges {
                target: image
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
                target: image
                property: "opacity"
                duration: 200
            }
            NumberAnimation {
                target: image
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
                        target: image
                        property: "opacity"
                        duration: 200
                    }
                    NumberAnimation {
                        target: image
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
                        target: image
                        property: "opacity"
                        duration: 200
                    }
                    NumberAnimation {
                        target: image
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
                        target: image
                        property: "opacity"
                        duration: 200
                    }
                    NumberAnimation {
                        target: image
                        property: "anchors.topMargin"
                        duration: 100
                    }
                }
            }
        }
    ]
}
