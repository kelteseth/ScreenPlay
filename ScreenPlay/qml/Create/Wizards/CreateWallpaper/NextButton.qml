import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

Button {
    id: root
    text: qsTr("Next")
    state: "disabled"
    Material.background: Material.Orange
    Material.foreground: "white"

    states: [
        State {
            name: "enabled"
            PropertyChanges {
                target: root
                text: qsTr("Finish")
            }
        },
        State {
            name: "disabled"
            PropertyChanges {
                target: root
                text: qsTr("Creating")
                Material.background: Material.Grey
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
