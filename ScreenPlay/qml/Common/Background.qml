import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Rectangle {
    id: element
    anchors.fill: parent
    color: Material.theme === Material.Light ? "white" : Qt.darker(
                                                   Material.background)
    state: "init"

    Rectangle {
        id: bgCommunity
        anchors.fill: parent
    }
    Rectangle {
        id: bgWorkshop
        color: "#161C1D"
        anchors.fill: parent
    }

    states: [
        State {
            name: "init"
            PropertyChanges {
                target: bgCommunity
                opacity: 0
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 0
            }
        },
        State {
            name: "create"
            PropertyChanges {
                target: bgCommunity
                opacity: 0
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 0
            }
        },
        State {
            name: "community"
            PropertyChanges {
                target: bgCommunity
                opacity: 1
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 0
            }
        },
        State {
            name: "workshop"
            PropertyChanges {
                target: bgCommunity
                opacity: 0
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 1
            }
        }
    ]

    transitions: [

        Transition {
            from: "*"
            to: "*"

            PropertyAnimation {
                targets: [bgCommunity, bgWorkshop]
                property: "opacity"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        }
    ]
}
