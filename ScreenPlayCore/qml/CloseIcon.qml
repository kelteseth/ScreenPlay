import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Material
/*!
   \qmltype Close Icon
   \brief A image selector with popup preview.
*/
MouseArea {
    id: root
    /*!
        \qmlproperty color BackButtonIcon::color
        Color if the icon.
    */
    property color color: Material.iconColor
    /*!
        \qmlproperty string BackButtonIcon::icon
        Icon image if the icon.
    */
    property string icon: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg"
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
    MultiEffect {
        id: iconColorOverlay
        anchors.fill: imgClose
        source: imgClose
        colorizationColor: root.color
        colorization: 1.0
    }
    states: [
        State {
            name: "hover"
            PropertyChanges {
                target: iconColorOverlay
                colorizationColor: Material.color(Material.Orange)
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
                property: "colorizationColor"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
