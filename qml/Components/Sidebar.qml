import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {
    id:sidebar
    height: 768
    width:400
    state: "inactive"
    focus: true

    property string activeScreen: ""

    onActiveScreenChanged:{
        print(activeScreen)

    }


    Item {
        id: sidebarWrapper

        anchors {
            top:sidebar.top
            right:sidebar.right
            bottom:sidebar.bottom
            left:sidebar.left
        }

        Rectangle {
            id:sidebarBackground
            color: "grey"
            anchors {
                top:parent.top
                right:parent.right
                bottom:parent.bottom
                left:parent.left
                leftMargin: 5
            }

            Rectangle {
                id: rectangle
                y: 0
                height: 237
                color: "#2b2b2b"
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
            }

            Button {
                id: button
                text: qsTr("Back")
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                onClicked: {
                    sidebar.state = sidebar.state === "active" ? "inactive" : "active"
                }
            }

            Button {
                id: button1
                y: 710
                text: qsTr("Set wallpaper")
                anchors.left: parent.left
                anchors.leftMargin: 148
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                onClicked: {

                }
            }

        }

        Rectangle {
            id:shadow
            anchors {
                top:parent.top
                right:sidebarBackground.left
                bottom:parent.bottom
                left:parent.left
            }


            LinearGradient {
                anchors.fill: parent
                start: Qt.point(0, 0)
                end: Qt.point(0, 5)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#00000000" }
                    GradientStop { position: 1.0; color: "#22000000" }
                }
            }

        }

    }


    states: [
        State {
            name: "active"

            PropertyChanges {
                target: sidebarWrapper
                anchors.leftMargin: 0
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                target: sidebarWrapper
                anchors.leftMargin: sidebar.width
            }
        }
    ]

    transitions: [
        Transition {
            to: "active"

            NumberAnimation {
                properties: "anchors.leftMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            to: "inactive"

            NumberAnimation {
                properties: "anchors.leftMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]

}
