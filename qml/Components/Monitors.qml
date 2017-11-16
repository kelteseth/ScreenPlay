import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

Item {
    id: monitors
    state: "inactive"

    property string activeMonitorName: ""

    onStateChanged: {
        monitorWrapper.focus = monitors.state == "active" ? true : false
        if(monitors.state == "active"){
             monitorListModel.reloadMonitors();
        }
    }

    Rectangle {
        id: background
        color: "#80000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
        }
    }

    Rectangle {
        color: "white"
        width: 900
        height: 600
        z: 98
        anchors.centerIn: parent

        Rectangle {
            color: "gray"
            anchors.centerIn: parent
            id: monitorWrapper
            width: 200
            height: 100
        }

        MonitorSelection {
            id: monitorSelection
            anchors.centerIn: parent
            anchors.fill: parent
            availableWidth: monitorWrapper.width
            availableHeight: monitorWrapper.height
        }

        Button {
            id: btn
            text: "Remove all wallpaper"
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
                target: monitorWrapper
                anchors.topMargin: 0
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
                target: monitorWrapper
                anchors.topMargin: 50
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

            NumberAnimation {
                target: monitorWrapper
                property: "anchors.topMargin"
                duration: 200
            }
        }
    ]
}
