import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id: navigationItem
    width: txtAmount.paintedWidth + txt.paintedWidth + icon.paintedWidth + 40
    Behavior on width {
        PropertyAnimation {
            duration: 50
        }
    }
    height: 60
    state: "inactive"
    clip: true

    property string iconSource: "qrc:/assets/icons/icon_installed.svg"
    property alias name: txt.text
    property alias amount: txtAmount.text

    property bool enabled: true
    onEnabledChanged: {
        if (!enabled) {
            navigationItem.width = 0
            navigationItem.opacity = 0
        }
    }

    signal pageClicked(string name)

    function setActive(isActive) {
        if (isActive) {
            navigationItem.state = "active"
        } else {
            navigationItem.state = "inactive"
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        cursorShape: Qt.PointingHandCursor
        onClicked: {
            navigationItem.pageClicked(navigationItem.name)
        }

        Image {
            id: icon
            source: iconSource
            width: 16
            height: 16
            sourceSize.height: 16
            sourceSize.width: 16
            fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: txtAmount
            anchors.left: icon.right
            anchors.leftMargin: 10
            font.pointSize: 14
            color: "#626262"
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Roboto"
            font.weight: Font.Normal
            text: ""

        }

        Text {
            id: txt
            anchors.left: txtAmount.right
            anchors.leftMargin: navigationItem.amount == "" ? 0 : 5
            text: "name"
            font.pointSize: 14
            color: "#626262"
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Roboto"
            font.weight: Font.Normal
        }

        ColorOverlay {
            id: iconColorOverlay
            anchors.fill: icon
            source: icon
            color: "#FFAB00"
        }

        Rectangle {
            id: navIndicator
            y: 83
            height: 3
            color: "#FFAB00"
            anchors.left: parent.left
            anchors.right: parent.right
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
                color: "#00000000"
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "active"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 200
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "*"
            to: "disabled"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 200
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "*"
            to: "inactive"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 100
                easing.type: Easing.OutQuart
            }
        }
    ]
}
