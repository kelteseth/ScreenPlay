import QtQuick
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects

Rectangle {
    id: element

    anchors.fill: parent
    color: Material.theme === Material.Light ? "white" : Qt.darker(Material.background)
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
