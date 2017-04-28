import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: monitors
    state:"inactive"

    property string activeMonitorName: ""

    Component.onCompleted: {


    }

    onStateChanged: {
        monitorWrapper.focus = monitors.state == "active" ? true : false
    }

    Rectangle {
        id:background
        color: "#80000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
        }
    }


    Rectangle {
        id:monitorWrapper
        width: 900
        height: 600
        radius: 6
        z:99
        anchors.centerIn: parent

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            Rectangle {
                id: rectangle
                height: 464
                color: "#dfdfdf"
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.top: parent.top
                anchors.topMargin: 20

                Row {



                Repeater {
                    id:rp
                    anchors.fill: parent
                    model:monitorListModel
                    delegate: Item {
                        height: 200
                        width: 200
                        Column {
                           spacing: 5

                           Text {
                               text: monitorNumber
                               anchors.horizontalCenter: parent.horizontalCenter
                           }


                           Text {
                               text: monitorAvailableGeometryRole.x + " " + monitorAvailableGeometryRole.y
                               anchors.horizontalCenter: parent.horizontalCenter
                           }

                        }
                    }
                }
                }
            }
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
                enabled:true
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
                enabled:false
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
