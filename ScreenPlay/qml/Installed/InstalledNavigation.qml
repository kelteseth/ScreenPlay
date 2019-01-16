import QtQuick 2.9
import QtGraphicalEffects 1.0

Item {
    id: navigationItem
    width: 150
    height: 50
    state: "inactive"
    clip: true

    property string name: "value"
    onNameChanged: {
        txt.text = name
        textMetrics.text = name
        item2.width = textMetrics.width + 30
    }

    property string iconSource: "qrc:/assets/icons/icon_installed.svg"

    signal pageClicked(string name)

    function setActive(isActive) {
        if (isActive) {
            navigationItem.state = "active"
        } else {
            navigationItem.state = "inactive"
        }
    }

    TextMetrics {
        id: textMetrics
        font.pointSize: 14
        font.family: "Roboto"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            navigationItem.pageClicked(navigationItem.name)
        }

        Item {
            id: item2

            width: 80
            height: 31
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                id: txt
                anchors.left: icon.right
                anchors.leftMargin: 10
                text: "name"
                font.pointSize: 12
                color: "#626262"
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Roboto"
                font.weight: Font.Normal
                
            }

            Image {
                id: icon
                source: iconSource
                width: 14
                height: 14
                sourceSize.height: 14
                sourceSize.width: 14
                fillMode: Image.PreserveAspectFit
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.verticalCenter: parent.verticalCenter
            }

            ColorOverlay {
                id: iconColorOverlay
                anchors.fill: icon
                source: icon
                color: "#FFAB00"
            }
        }

        Rectangle {
            id: navIndicator
            y: 83
            height: 3
            color: "#FFAB00"
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: navIndicator
                anchors.bottomMargin: 0
            }

            PropertyChanges {
                target: iconColorOverlay
                color: "#FFAB00"
            }
        },
        State {
            name: "disabled"

            PropertyChanges {
                target: navIndicator
                anchors.bottomMargin: -3
            }

            PropertyChanges {
                target: iconColorOverlay
                color: "#00000000"
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                target: navIndicator
                anchors.bottomMargin: -3
            }

            PropertyChanges {
                target: iconColorOverlay
                color: "#BEBEBE"
            }
        }
    ]

    transitions: [
        Transition {
            to: "active"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 100
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            to: "inactive"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 100
                easing.type: Easing.OutQuart
            }
        }
    ]
}
