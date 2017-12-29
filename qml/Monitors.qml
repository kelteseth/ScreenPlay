import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2

Item {
    id: monitors
    state: "inactive"
    focus: true

    property string activeMonitorName: ""

    onStateChanged: {
        bgMouseArea.focus = monitors.state == "active" ? true : false
        if (monitors.state == "active") {

            //monitorListModel.reloadMonitors();
        }
    }

    Rectangle {
        id: background
        color: "#80000000"
        anchors.fill: parent

        MouseArea {
            id:bgMouseArea
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
            hoverEnabled: true
        }
    }

    Rectangle {
        color: "white"
        width: 900
        radius: 3
        height: 600
        z: 98
        anchors.centerIn: parent

        MonitorSelection {
            id: monitorSelection
            anchors{
                top: parent.top
                topMargin: 60
                horizontalCenter: parent.horizontalCenter
            }
            width:600
            availableWidth: 600
            availableHeight: 600
        }

        Button {
            id: btn
            text: qsTr("Remove all wallpaper")
            Material.background: Material.Orange
            Material.foreground: "white"
            onClicked: screenPlaySettings.removeAll(
                           ) //screenPlaySettings.removeWallpaperAt(monitorSelection.activeMonitorIndex)
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 30
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
        }
    ]

    transitions: [
        Transition {

            NumberAnimation {
                target: background
                property: "opacity"
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
