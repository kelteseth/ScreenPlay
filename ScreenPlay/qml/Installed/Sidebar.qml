import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../Monitors"

Item {
    id: sidebar
    width: 400
    state: "inactive"
    focus: true

    property real navHeight
    property string type
    property string activeScreen

    Connections {
        target: utility

        onSetSidebarItem: {

            // Toggle sidebar if clicked on the same content twice
            if (activeScreen === screenId && sidebar.state !== "inactive") {
                sidebar.state = "inactive"
                return
            }

            activeScreen = screenId
            sidebar.type = type


            switch (type) {
            case "videoWallpaper":
                state = "activeScene"
                return
            case "htmlWallpaper":
                state = "activeScene"
                return
            case "qmlWallpaper":
                state = "activeScene"
                return
            case "godotWallpaper":
                state = "activeScene"
                return
            case "qmlWidget":
                state = "activeWidget"
                return
            case "htmlWidget":
                state = "activeWidget"
                return
            case "standaloneWidget":
                state = "activeWidget"
                return
            }
        }
    }

    MouseArea {
        id: mouseAreaHelper
        anchors.fill: parent
        enabled: true
    }

    onActiveScreenChanged: {
        txtHeadline.text = installedListModel.get(activeScreen).screenTitle
        image.source = Qt.resolvedUrl(
                    installedListModel.absoluteStoragePath + "/" + activeScreen
                    + "/" + installedListModel.get(activeScreen).screenPreview)
    }

    Item {
        id: sidebarWrapper

        anchors {
            top: sidebar.top
            right: sidebar.right
            bottom: sidebar.bottom
            left: sidebar.left
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

        Rectangle {
            id: sidebarBackground
            color: "white"
            anchors {
                top: navBackground.bottom
                right: parent.right
                bottom: parent.bottom
                left: parent.left
                leftMargin: 5
            }

            Rectangle {
                id: imageWrapper
                height: 237
                color: "#2b2b2b"
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0

                Image {
                    id: image
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
                    height: 20
                    cached: true

                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        left: parent.left
                    }
                    start: Qt.point(0, 30)
                    end: Qt.point(0, 0)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "#88000000"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#00000000"
                        }
                    }
                }
            }

            MouseArea {
                id: button
                height: 50
                width: 50
                anchors.top: parent.top
                anchors.left: parent.left
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    sidebar.state = sidebar.state === "active" ? "inactive" : "active"
                }

                Image {
                    id: imgBack
                    source: "qrc:/assets/icons/icon_arrow_right.svg"
                    sourceSize: Qt.size(15, 15)
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                }
            }

            Item {
                id: headlineWrapper
                height: 60
                anchors {
                    top: imageWrapper.bottom
                    topMargin: 10
                    right: parent.right
                    rightMargin: 10
                    left: parent.left
                    leftMargin: 10
                }

                Text {
                    id: txtHeadline
                    text: qsTr("Headline")
                    height: 60

                    font.family: "Roboto"
                    font.weight: Font.Thin
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 16
                    color: "#2F2F2F"
                    wrapMode: Text.WrapAnywhere
                    anchors {
                        top: parent.top
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 20
                    }
                }
            }

            Text {
                id: txtHeadlineMonitor
                text: qsTr("Select a Monitor to display the content")
                height: 50

                horizontalAlignment: Qt.AlignHCenter
                font.family: "Roboto"
                font.pointSize: 14
                color: "#2F2F2F"
                wrapMode: Text.WrapAnywhere
                anchors {
                    top: headlineWrapper.bottom
                    margins: 20
                    right: parent.right
                    left: parent.left
                }
            }

            Rectangle {
                id: monitorSelectionWrapper
                height: 80
                width: 400
                anchors {
                    top: txtHeadlineMonitor.bottom
                    right: parent.right
                    left: parent.left
                }

                MonitorSelection {
                    id: monitorSelection
                    width: 360
                    height: parent.height
                    availableWidth: 360
                    fontSize: 11
                    availableHeight: 50
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Item {
                id: sliderVolumeWrapper
                height: 100
                width: 400
                anchors {
                    bottom: btnSetWallpaper.top
                    bottomMargin: 20
                    right: parent.right
                    rightMargin: 30
                    leftMargin: 30
                    left: parent.left
                }
                Text {
                    id: txtSliderVolume
                    text: qsTr("Volume")
                    height: 30

                    font.family: "Roboto"
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 10
                    color: "#626262"
                    wrapMode: Text.WrapAnywhere
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                }
                Row {
                    id: sliderVolumeWrapperBottom
                    height: 70
                    spacing: 30
                    width: 300
                    anchors {
                        bottom: parent.bottom
                        horizontalCenter: parent.horizontalCenter
                    }
                    Image {
                        width: 20
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/assets/icons/icon_volume.svg"
                        sourceSize: Qt.size(20, 20)
                    }

                    Slider {
                        id: sliderVolume
                        stepSize: 0.01
                        from: 0
                        value: 1
                        to: 1
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        id: name
                        color: "#818181"
                        text: Math.round(sliderVolume.value * 100) / 100
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "Libre Baskerville"
                        font.pointSize: 12
                        font.italic: true
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            Text {
                visible: false
                id: txtComboBoxFillMode
                text: qsTr("Fill Mode")
                height: 30

                font.family: "Roboto"
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 10
                color: "#626262"
                wrapMode: Text.WrapAnywhere
                anchors {
                    top: sliderVolumeWrapper.bottom
                    left: parent.left
                    leftMargin: 30
                }
            }

            ComboBox {
                visible: false
                id: settingsComboBox
                width: 200

                anchors {
                    top: txtComboBoxFillMode.bottom
                    topMargin: 10

                    horizontalCenter: parent.horizontalCenter
                }

                model: ListModel {
                    ListElement {
                        text: "Stretch"
                    }
                    ListElement {
                        text: "PreserveAspectFit"
                    }
                    ListElement {
                        text: "PreserveAspectCrop"
                    }
                }
            }

            Button {
                id: btnSetWallpaper
                text: qsTr("Set wallpaper")
                Material.accent: Material.Orange
                Material.background: Material.Orange
                Material.foreground: "white"

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

                    if (type.endsWith("Wallpaper")) {
                        screenPlay.createWallpaper(
                                    monitorSelection.activeMonitorIndex, installedListModel.absoluteStoragePath + "/" + activeScreen,
                                    installedListModel.get(activeScreen).screenPreview,
                                    (Math.round(sliderVolume.value * 100) / 100),
                                    settingsComboBox.model.get(settingsComboBox.currentIndex).text.toString(
                                        ), type)
                    } else {
                        screenPlay.createWidget(
                                    installedListModel.absoluteStoragePath + "/" + activeScreen,
                                    installedListModel.get(
                                        activeScreen).screenPreview)
                    }
                    sidebar.state = "inactive"
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
            name: "active"

            PropertyChanges {
                target: mouseAreaHelper
                enabled: true
            }

            PropertyChanges {
                target: sidebar
                anchors.rightMargin: 0
            }
            PropertyChanges {
                target: image
                opacity: 1
                anchors.topMargin: 0
            }
            PropertyChanges {
                target: btnSetWallpaper
                text: qsTr("Set Wallpaper")
            }
            PropertyChanges {
                target: txtHeadlineMonitor
                opacity: 1
            }
            PropertyChanges {
                target: monitorSelectionWrapper
                opacity: 1
            }
            PropertyChanges {
                target: sliderVolumeWrapper
                opacity: 1
            }
            PropertyChanges {
                target: txtComboBoxFillMode
                opacity: 1
            }
            PropertyChanges {
                target: settingsComboBox
                opacity: 1
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                target: mouseAreaHelper
                enabled: false
            }

            PropertyChanges {
                target: sidebar
                anchors.rightMargin: -sidebar.width
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
                target: image
                opacity: 1
                anchors.topMargin: 0
            }

            PropertyChanges {
                target: headlineWrapper
                opacity: 1
                enabled: true
            }
            PropertyChanges {
                target: btnSetWallpaper
                text: qsTr("Set Widget")
            }
            PropertyChanges {
                target: txtHeadlineMonitor
                opacity: 0
            }
            PropertyChanges {
                target: monitorSelectionWrapper
                opacity: 0
            }
            PropertyChanges {
                target: sliderVolumeWrapper
                opacity: 0
            }
            PropertyChanges {
                target: txtComboBoxFillMode
                opacity: 0
            }
            PropertyChanges {
                target: settingsComboBox
                opacity: 0
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
                target: headlineWrapper
                opacity: 1
                enabled: true
            }
            PropertyChanges {
                target: btnSetWallpaper
                text: qsTr("Set Scene")
            }
            PropertyChanges {
                target: txtHeadlineMonitor
                opacity: 1
            }
            PropertyChanges {
                target: monitorSelectionWrapper
                opacity: 1
            }
            PropertyChanges {
                target: sliderVolumeWrapper
                opacity: 0
            }
            PropertyChanges {
                target: txtComboBoxFillMode
                opacity: 0
            }
            PropertyChanges {
                target: settingsComboBox
                opacity: 0
            }
        }
    ]

    transitions: [
        Transition {
            to: "active"

            SequentialAnimation {
                NumberAnimation {
                    target: sidebar
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
            to: "inactive"

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
                target: sidebar
                properties: "anchors.rightMargin"
                duration: 250
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            to: "activeWidget"

            SequentialAnimation {
                NumberAnimation {
                    target: sidebar
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
                    target: sidebar
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
