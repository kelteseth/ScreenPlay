import QtQuick

Rectangle {
    id: root
    implicitHeight: 100
    implicitWidth: 100
    property int animationDuration: 5000
    property bool running: true
    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop {
            id: stop1
            position: -0.25
            color: "#f79533"
        }
        GradientStop {
            id: stop2
            position: -0.11
            color: "#f37055"
        }
        GradientStop {
            id: stop3
            position: 0.03
            color: "#ef4e7b"
        }
        GradientStop {
            id: stop4
            position: 0.17
            color: "#a166ab"
        }
        GradientStop {
            id: stop5
            position: 0.31
            color: "#5073b8"
        }
        GradientStop {
            id: stop6
            position: 0.45
            color: "#1098ad"
        }
        GradientStop {
            id: stop7
            position: 0.59
            color: "#07b39b"
        }
        GradientStop {
            id: stop8
            position: 0.75
            color: "#6fba82"
        }
    }
    SequentialAnimation {
        running: root.running
        loops: Animation.Infinite
        ParallelAnimation {
            NumberAnimation {
                target: stop1
                property: "position"
                from: -0.25
                to: 0.25
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop2
                property: "position"
                from: -0.11
                to: 0.39
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop3
                property: "position"
                from: 0.03
                to: 0.53
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop4
                property: "position"
                from: 0.17
                to: 0.67
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop5
                property: "position"
                from: 0.31
                to: 0.81
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop6
                property: "position"
                from: 0.45
                to: 0.95
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop7
                property: "position"
                from: 0.59
                to: 1.09
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop8
                property: "position"
                from: 0.75
                to: 1.25
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
        }
        ParallelAnimation {
            NumberAnimation {
                target: stop1
                property: "position"
                from: 0.25
                to: -0.25
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop2
                property: "position"
                from: 0.39
                to: -0.11
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop3
                property: "position"
                from: 0.53
                to: 0.03
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop4
                property: "position"
                from: 0.67
                to: 0.17
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop5
                property: "position"
                from: 0.81
                to: 0.31
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop6
                property: "position"
                from: 0.95
                to: 0.45
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop7
                property: "position"
                from: 1.09
                to: 0.59
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: stop8
                property: "position"
                from: 1.25
                to: 0.75
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
}
