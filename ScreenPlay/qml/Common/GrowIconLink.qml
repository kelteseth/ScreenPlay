import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.12

Rectangle {
    id: root

    property alias iconSource: icon.source
    property string url
    property alias color: overlay.color

    color: Material.theme === Material.Light ? Material.background : Qt.darker(Material.background)
    width: 42
    height: width
    radius: width

    Image {
        id: icon

        sourceSize: Qt.size(28, 28)
        anchors.centerIn: parent
        visible: false
        smooth: true
        source: "qrc:/assets/icons/icon_info.svg"
    }

    ColorOverlay {
        id: overlay

        anchors.fill: icon
        source: icon
        color: Material.accent
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
                width: 34
                height: 34
                sourceSize: Qt.size(34, 34)
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
