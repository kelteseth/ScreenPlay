import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3


import ScreenPlay 1.0
import "../Common/" as SP

Item {
    id: monitors
    state: "inactive"
    focus: true

    property string activeMonitorName: ""
    property int activeMonitorIndex


    onStateChanged: {
        bgMouseArea.focus = monitors.state == "active" ? true : false
        if (monitors.state === "active") {
            ScreenPlay.screenPlayManager.requestProjectSettingsListModelAt(0)
        }
    }

    Rectangle {
        id: background
        color: "#99000000"
        anchors.fill: parent

        MouseArea {
            id: bgMouseArea
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
        }
    }

    Rectangle {
        id: monitorsSettingsWrapper
        color: "white"
        radius: 3
        z: 98
        width: 1000
        height: 500
        clip: true
        anchors {
            top: parent.top
            topMargin: 50
            horizontalCenter: parent.horizontalCenter
            margins: 50
        }

        Item {
            id: itmLeftWrapper
            width: parent.width * .5
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
                color: "#626262"
                font.family: "Roboto"
                font.weight: Font.Thin
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
                background: "#E4E4E4"
                radius: 3
                height: 200
                z: 99
                width: parent.width * .9
                multipleMonitorsSelectable: false
                anchors {
                    top: txtHeadline.bottom
                    topMargin: 20
                    left: parent.left
                    leftMargin: 20
                }
                availableWidth: width - 20
                availableHeight: 150
                onRequestProjectSettings: {
                    // This will return in the connection with target: screenPlay
                    ScreenPlay.screenPlayManager.requestProjectSettingsListModelAt(at)
                    activeMonitorIndex = at
                }
                Connections {
                    target: ScreenPlay.screenPlayManager
                    onProjectSettingsListModelFound: {
                        videoControlWrapper.state = "visible"
                        customPropertiesGridView.model = li
                        if (type == "videoWallpaper") {
                            customPropertiesGridView.state = "hidden"
                            videoControlWrapper.state = "visible"
                        } else {
                            customPropertiesGridView.state = "visible"
                            videoControlWrapper.state = "hidden"
                        }
                    }
                    onProjectSettingsListModelNotFound: {
                        customPropertiesGridView.model = null
                        videoControlWrapper.state = "hidden"
                        customPropertiesGridView.state = "hidden"
                    }
                }
            }

            Row {
                anchors {
                    top: monitorSelection.bottom
                    right: parent.right
                    bottom: parent.bottom
                    left: parent.left
                    margins: 20
                }
                spacing: 20
                Button {
                    id: btnRemoveSelectedWallpaper
                    text: qsTr("Remove selected")
                    Material.background: Material.Orange
                    Material.foreground: "white"
                    enabled: monitorSelection.activeMonitors.length == 1
                    onClicked: {
                        ScreenPlay.screenPlayManager.removeWallpaperAt(monitorSelection.activeMonitors[0])
                        monitorSelection.deselectAll()
                    }
                }
                Button {
                    id: btnRemoveAllWallpaper
                    text: qsTr("Remove all Wallpapers")
                    Material.background: Material.Orange
                    Material.foreground: "white"
                    onClicked: {
                        ScreenPlay.screenPlayManager.removeAllWallpapers()
                        monitors.state = "inactive"
                    }
                }
            }
        }

        DefaultVideoControls {
            id: videoControlWrapper
            activeMonitorIndex: monitors.activeMonitorIndex
            anchors {
                top: parent.top
                topMargin: 60
                right: parent.right
                bottom: parent.bottom
                margins: 30
                left: itmLeftWrapper.right
            }
        }

        GridView {
            id: customPropertiesGridView
            boundsBehavior: Flickable.DragOverBounds
            maximumFlickVelocity: 7000
            flickDeceleration: 5000
            cellWidth: 340
            cellHeight: 50
            cacheBuffer: 10000
            clip: true

            anchors {
                top: parent.top
                topMargin: 60
                right: parent.right
                bottom: parent.bottom
                margins: 30
                left: itmLeftWrapper.right
            }

            delegate: MonitorsProjectSettingItem {
                id: delegate
                selectedMonitor: monitors.activeMonitorIndex
            }

            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
            }

            states: [
                State {
                    name: "visible"
                    PropertyChanges {
                        target: customPropertiesGridView
                        opacity: 1
                        z:1
                        anchors.topMargin: 60
                    }
                },
                State {
                    name: "hidden"
                    PropertyChanges {
                        target: customPropertiesGridView
                        opacity: 0
                        z:-1
                        anchors.topMargin: -100
                    }
                }
            ]

            transitions: [
                Transition {
                    from: "visible"
                    to: "hidden"
                    reversible: true
                    PropertyAnimation {
                        target: customPropertiesGridView
                        duration: 300
                        easing.type: Easing.InOutQuart
                        properties: "anchors.topMargin, opacity"
                    }
                }
            ]
        }

        MouseArea {
            anchors {
                top: parent.top
                right: parent.right
                margins: 5
            }
            width: 32
            height: width
            cursorShape: Qt.PointingHandCursor
            onClicked: monitors.state = "inactive"

            Image {
                id: imgClose
                source: "qrc:/assets/icons/font-awsome/close.svg"
                width: 16
                height: 16
                anchors.centerIn: parent
                sourceSize: Qt.size(width, width)
            }
            ColorOverlay {
                id: iconColorOverlay
                anchors.fill: imgClose
                source: imgClose
                color: "gray"
            }
        }
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: monitors
                opacity: 1
                enabled: true
            }

            PropertyChanges {
                target: background
                opacity: 1
            }

            PropertyChanges {
                target: monitorsSettingsWrapper
                anchors.topMargin: 50
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: monitors
                opacity: 0
                enabled: false
            }

            PropertyChanges {
                target: background
                opacity: 0
            }
            PropertyChanges {
                target: monitorsSettingsWrapper
                anchors.topMargin: 150
            }
        }
    ]

    transitions: [
        Transition {
            from: "active"
            to: "inactive"
            reversible: true

            PropertyAnimation {
                target: background
                property: "opacity"
                duration: 200
                easing.type: Easing.OutQuart
            }

            PropertyAnimation {
                target: monitorsSettingsWrapper
                property: "anchors.topMargin"
                duration: 200
                easing.type: Easing.OutQuad
            }

            NumberAnimation {
                target: monitors
                property: "opacity"
                duration: 200
                easing.type: Easing.OutQuad
            }
        }
    ]
}
