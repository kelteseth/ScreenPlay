import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material

/*!
   \qmltype Window Nav Button
   \brief A image selector with popup preview.

*/
MouseArea {
    id: root

    height: 29
    width: 45

    /*!
        \qmlproperty color BackButtonIcon::color

        Color if the icon.
    */
    property color color: Material.iconColor
    /*!
        \qmlproperty string BackButtonIcon::icon

        Icon image if the icon.
    */
    property alias icon: img

    cursorShape: Qt.PointingHandCursor
    onEntered: root.state = "hover"
    onExited: root.state = ""
    hoverEnabled: true

    Image {
        id: img

        source: root.icon
        visible: false
        width: 14
        height: 14
        smooth: true
        anchors.centerIn: parent
        sourceSize: Qt.size(width, width)
    }

    ColorOverlay {
        id: iconColorOverlay

        anchors.fill: img
        source: img
        color: root.color
    }

    states: [
        State {
            name: "hover"

            PropertyChanges {
                target: iconColorOverlay
                color: Material.color(Material.Orange)
            }

        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "hover"
            reversible: true

            ColorAnimation {
                target: iconColorOverlay
                duration: 200
                easing.type: Easing.InOutQuad
            }

        }
    ]
}
