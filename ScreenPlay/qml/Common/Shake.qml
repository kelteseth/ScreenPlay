import QtQuick

Translate {
    id: root

    property int offset: 0
    property int loops: 3
    property int loopOffset: 1000
    property SequentialAnimation shake

    function start(offset = 0, loopOffset = 1000, loops = 1) {
        root.offset = offset;
        root.loopOffset = loopOffset;
        root.loops = loops;
        shake.restart();
    }

    shake: SequentialAnimation {
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
                to: 10
                duration: 50
                easing.type: Easing.InOutBounce
            }

            PropertyAnimation {
                target: root
                property: "x"
                from: 10
                to: -10
                duration: 100
                easing.type: Easing.InOutBounce
            }

            PropertyAnimation {
                target: root
                property: "x"
                from: -10
                to: 0
                duration: 50
            }

            PropertyAnimation {
                target: root
                property: "x"
                from: 0
                to: 10
                duration: 50
                easing.type: Easing.InOutBounce
            }

            PropertyAnimation {
                target: root
                property: "x"
                from: 10
                to: -10
                duration: 100
                easing.type: Easing.InOutBounce
            }

            PropertyAnimation {
                target: root
                property: "x"
                from: -10
                to: 0
                duration: 50
            }

        }

        PauseAnimation {
            duration: root.loopOffset
        }

    }

}
