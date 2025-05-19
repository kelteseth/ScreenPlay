import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Material

Rectangle {
    id: root
    property alias iconSource: icon.source
    property string url
    property alias iconColor: overlay.colorizationColor

    color: Material.theme === Material.Light ? Material.backgroundColor : Qt.darker(Material.backgroundColor)
    width: 42
    height: width
    radius: width

    Image {
        id: icon
        sourceSize: Qt.size(28, 28)
        anchors.centerIn: parent
        smooth: true
        source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_info.svg"
        visible: false
    }

    MultiEffect {
        id: overlay
        anchors.fill: icon
        source: icon
        colorization: 1.0
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
