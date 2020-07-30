import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0
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
            ScreenPlay.screenPlayManager.requestProjectSettingsAtMonitorIndex(0)
        }
    }

    Rectangle {
        id: background
        color: "#cc000000"
        anchors.fill: parent

        MouseArea {
            id: bgMouseArea
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
        }
    }

    Rectangle {
        id: monitorsSettingsWrapper
        color: Material.theme === Material.Light ? "white" : Material.background
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
                color: Material.primaryTextColor
                font.family: ScreenPlay.settings.font
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
                    ScreenPlay.screenPlayManager.requestProjectSettingsAtMonitorIndex(
                                at)
                    activeMonitorIndex = at
                }
                Connections {
                    target: ScreenPlay.screenPlayManager
                    function onProjectSettingsListModelResult(listModel) {
                        customPropertiesGridView.model = listModel
                    }
                }
            }

            ColumnLayout {
                anchors {
                    top: monitorSelection.bottom
                    right: parent.right
                    left: parent.left
                    margins: 20
                }
                spacing: 5
                Button {
                    id: btnRemoveSelectedWallpaper
                    text: qsTr("Remove selected")
                    Material.background: Material.accent
                    Material.foreground: "white"
                    font.family: ScreenPlay.settings.font
                    enabled: monitorSelection.activeMonitors.length == 1
                    onClicked: {
                        ScreenPlay.screenPlayManager.removeWallpaperAt(
                                    monitorSelection.activeMonitors[0])
                    }
                }
                Button {
                    id: btnRemoveAllWallpaper
                    text: qsTr("Remove all Wallpapers")
                    Material.background: Material.accent
                    Material.foreground: "white"
                    font.family: ScreenPlay.settings.font
                    enabled: ScreenPlay.screenPlayManager.activeWallpaperCounter > 0
                    onClicked: {
                        ScreenPlay.screenPlayManager.removeAllWallpapers()
                        monitors.state = "inactive"
                    }
                }
                Button {
                    id: btnRemoveAllWidgets
                    text: qsTr("Remove all Widgets")
                    Material.background: Material.accent
                    Material.foreground: "white"
                    font.family: ScreenPlay.settings.font
                    enabled: ScreenPlay.screenPlayManager.activeWidgetsCounter > 0
                    onClicked: {
                        ScreenPlay.screenPlayManager.removeAllWidgets()
                        monitors.state = "inactive"
                    }
                }
            }
        }

        Rectangle {
            color: Material.theme === Material.Light ? Material.background : Qt.darker(
                                                           Material.background)
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

            GridView {
                id: customPropertiesGridView
                boundsBehavior: Flickable.DragOverBounds
                maximumFlickVelocity: 7000
                flickDeceleration: 5000
                cellWidth: 340
                cellHeight: 50
                cacheBuffer: 10000
                clip: true
                anchors.fill: parent
                anchors.margins: 10

                delegate: MonitorsProjectSettingItem {
                    id: delegate
                    width: parent.width - 40
                    selectedMonitor: activeMonitorIndex
                    name: m_name
                    isHeadline: m_isHeadline
                    value: m_value
                }

                ScrollBar.vertical: ScrollBar {
                    snapMode: ScrollBar.SnapOnRelease
                    policy: ScrollBar.AlwaysOn
                }
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

/*##^##
Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
##^##*/

