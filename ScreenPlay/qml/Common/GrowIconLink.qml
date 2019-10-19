import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    color: "gray"
    width: 32
    height: 32

    property alias iconSource: icon.source
    property string url
    property alias color: overlay.color

    Image {
        id: icon
        sourceSize: Qt.size(32, 32)
        anchors.centerIn: parent
        visible: false
        source: "qrc:/assets/icons/icon_info.svg"
    }

    ColorOverlay {
        id: overlay
        anchors.fill: icon
        source: icon
        color: "orange"
    }

    MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        onClicked: Qt.openUrlExternally(url)
        onEntered: root.state = "hover"
        onExited: root.state = ""
        cursorShape: Qt.PointingHandCursor
    }

    states: [
        State {
            name: "hover"
            PropertyChanges {
                target: icon
                width:40
                height:40
                sourceSize: Qt.size(40,40)
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "hover"
            reversible: true
            PropertyAnimation {
                target: icon
                properties: "width,height,sourceSize"
                duration: 200
                easing.type: Easing.InOutQuart
            }
        }
    ]
}
