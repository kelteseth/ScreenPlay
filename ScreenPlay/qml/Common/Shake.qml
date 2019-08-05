import QtQuick 2.12

Translate {
    id: root
    function start(offset = 0, loopOffset = 1000, loops = 1) {
        root.offset = offset
        root.loopOffset = loopOffset
        root.loops = loops
        shake.restart()
    }

    property int offset: 0
    property int loops: 3
    property int loopOffset: 1000

    property SequentialAnimation shake: SequentialAnimation {

        loops: root.loops
        alwaysRunToEnd: true

        PauseAnimation {
            duration: root.offset
        }

        SequentialAnimation {

            PropertyAnimation {
                target: root
                property: "x"
                from: 0
                to: 30
                duration:  75
                easing.type: Easing.InOutBounce
            }
            PropertyAnimation {
                target: root
                property: "x"
                from: 30
                to: -30
                duration: 160
                easing.type: Easing.InOutBounce
            }
            PropertyAnimation {
                target: root
                property: "x"
                from: -30
                to: 0
                duration:  75
            }

            PropertyAnimation {
                target: root
                property: "x"
                from: 0
                to: 30
                duration: 75
                easing.type: Easing.InOutBounce
            }
            PropertyAnimation {
                target: root
                property: "x"
                from: 30
                to: -30
                duration: 160
                easing.type: Easing.InOutBounce
            }
            PropertyAnimation {
                target: root
                property: "x"
                from: -30
                to: 0
                duration:  75
            }
        }
        PauseAnimation {
            duration: root.loopOffset
        }
    }
}
