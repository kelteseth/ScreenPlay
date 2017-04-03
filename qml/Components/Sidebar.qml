import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {
    id:sidebar
    height: 768
    width:400
    state: "inactive"

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

            Button {
                id: button
                text: qsTr("Button")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    sidebar.state = sidebar.state == "active" ? "inactive" : "active"
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
                end: Qt.point(5, 0)
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
                duration: 500
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            to: "inactive"

            NumberAnimation {
                properties: "anchors.leftMargin"
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }
    ]

}
