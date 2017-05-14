import QtQuick 2.7

DropArea {
    id:dropper
    state:"invisible"

    onEntered: {
        dropper.state = "visible"
    }

    onExited: {
        dropper.state = "invisible"
    }

    onDropped: {
        print(drop.urls)
        print(drop.urls)
    }



    Rectangle {
        id:fill
        anchors.fill: parent
        opacity: .8
    }
    states: [
        State {
            name: "visible"
        },
        State {
            name: "invisible"

            PropertyChanges {
                target: fill
                opacity: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "invisible"

            NumberAnimation {
                target: fill
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "invisible"
            to: "visible"

            NumberAnimation {
                target: fill
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]

}
