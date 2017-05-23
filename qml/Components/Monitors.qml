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

                Item {
                    id:monitorListCenter
                    width:0
                    height:0
                    anchors.centerIn: parent
                }



                Repeater {
                    id:rp
                    anchors.fill: parent
                    anchors.centerIn: parent
                    anchors.margins: 30
                    model:monitorListModel

                    delegate: Rectangle {
                        color:"steelblue"
                        height: monitorSize.height / 10
                        width: monitorSize.width / 10
                        x:monitorAvailableGeometry.x /10
                        y: monitorAvailableGeometry.y / 10
                        anchors.margins: 10

                        Column {
                           spacing: 5

                           Text {
                               text: monitorNumber
                               anchors.horizontalCenter: parent.horizontalCenter
                           }

                           Text {
                               text: monitorName
                               anchors.horizontalCenter: parent.horizontalCenter
                           }


                           Text {
                               text: monitorSize.width + " " + monitorSize.height
                               anchors.horizontalCenter: parent.horizontalCenter
                           }


                           Text {
                               text: monitorAvailableGeometry.x + " " + monitorAvailableGeometry.y
                               anchors.horizontalCenter: parent.horizontalCenter
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
