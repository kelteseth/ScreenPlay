import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import "Controls/" as SP

Item {
    id: monitors
    state: "inactive"
    focus: true

    property string activeMonitorName: ""
    property int activeMonitorIndex

    onStateChanged: {
        bgMouseArea.focus = monitors.state == "active" ? true : false
        if (monitors.state === "active") {
            screenPlay.requestProjectSettingsListModelAt(0)
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
                font.pixelSize: 21
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
                    screenPlay.requestProjectSettingsListModelAt(at)
                    activeMonitorIndex = at
                }
                Connections {
                    target: screenPlay
                    onProjectSettingsListModelFound: {
                        gridView.model = li
                        // TODO via states
                        if (type == "video") {
                            videoControlWrapper.z = 10
                            gridView.z = 0
                            videoControlWrapper.visible = true
                            gridView.visible = false
                        } else {
                            videoControlWrapper.visible = false
                            gridView.visible = true
                            videoControlWrapper.z = 0
                            gridView.z = 10
                        }
                    }
                    onProjectSettingsListModelNotFound: {
                        gridView.model = null
                    }
                }
            }

            Button {
                id: btnRemoveAllWallpaper
                text: qsTr("Remove all wallpaper")
                Material.background: Material.Orange
                Material.foreground: "white"
                onClicked: {
                    screenPlay.removeAllWallpaper()
                    monitors.state = "inactive"
                }

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 30
                }
            }
        }
        Column {
            id: videoControlWrapper
            anchors {
                top: parent.top
                topMargin: 60
                right: parent.right
                bottom: parent.bottom
                margins: 30
                left: itmLeftWrapper.right
            }

            SP.Slider {
                headline: qsTr("Volume")
                onValueChanged: screenPlay.setWallpaperValue(activeMonitorIndex,"volume", value)
            }
            SP.Slider {
                headline: qsTr("Playback rate")
                onValueChanged: screenPlay.setWallpaperValue(activeMonitorIndex,"playbackRate", value)
            }
        }

        GridView {
            id: gridView
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
                selectedMonitor: monitorSelection.activeMonitorIndex
            }

            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
            }
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

            NumberAnimation {
                target: background
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: monitorsSettingsWrapper
                property: "anchors.topMargin"
                duration: 200
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: monitors
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
