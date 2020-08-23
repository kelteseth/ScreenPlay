import QtQuick 2.14
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3


/*!
   \qmltype Close Icon
   \brief A image selector with popup preview.

*/
MouseArea {
    id: root
    width: 32
    height: width
    cursorShape: Qt.PointingHandCursor

    onEntered: root.state = "hover"
    onExited: root.state = ""
    hoverEnabled: true
    anchors {
        top: parent.top
        right: parent.right
        margins: 10
    }


    /*!
        \qmlproperty color BackButtonIcon::color

        Color if the icon.
    */
    property color color: Material.iconColor


    /*!
        \qmlproperty string BackButtonIcon::icon

        Icon image if the icon.
    */
    property string icon: "qrc:/assets/icons/icon_close.svg"

    Image {
        id: imgClose
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
        anchors.fill: imgClose
        source: imgClose
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
