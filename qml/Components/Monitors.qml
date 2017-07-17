import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    id: monitors
    state: "inactive"

    property string activeMonitorName: ""

    onStateChanged: {
        monitorWrapper.focus = monitors.state == "active" ? true : false
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
        id: monitorWrapper
        width: 900
        height: 600

        z: 98
        anchors.centerIn: parent
        MonitorSelection {
            anchors.centerIn: parent
            anchors.fill: parent
            availableWidth: monitorWrapper.width
            availableHeight: monitorWrapper.height
        }
    }

    DropShadow {
        anchors.fill: monitorWrapper
        radius: 12.0
        cached: true
        samples: 17
        color: "#80000000"
        source: monitorWrapper
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
