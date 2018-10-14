import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

Button {
    id: root
    text: qsTr("Next")
    Material.background: Material.Orange
    Material.foreground: "white"

    icon.source: "qrc:/assets/icons/icon_arrow_right.svg"
    icon.color: "white"
    icon.width: 16
    icon.height: 16

    states: [
        State {
            name: "enabled"
            PropertyChanges {
                target: root
            }
        },
        State {
            name: "diabledPage0"
            PropertyChanges {
                target: root
                text: qsTr("Creating preview")
                Material.background: Material.Grey
            }
        },
        State {
            name: "diabledPage0NoTitle"
            PropertyChanges {
                target: root
                text: qsTr("No title set!")
                Material.background: Material.Grey
            }
        },
        State {
            name: "enabledPage0"
            PropertyChanges {
                target: root
                text: qsTr("Next")
                Material.background: Material.Orange
            }
        }
    ]
// TODO find  a way to smoothly change with on text change
//    transitions: [
//        Transition {
//            from: "*"
//            to: "*"
//            NumberAnimation {
//                property: "width"
//                easing.type: Easing.InOutQuad
//                duration: 2000
//            }
//        }
//    ]
}
