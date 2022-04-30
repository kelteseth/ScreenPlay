import QtQuick

Scale {
    id: root

    property int offset: 0
    property int loopOffset: 1000
    property int loops: 1
    property real cScale: 1.5
    property alias centerX: root.origin.x
    property alias centerY: root.origin.y
    property SequentialAnimation grow

    function start(offset = 0, loopOffset = 1000, scale = 1.5, loops = 1) {
        root.offset = offset;
        root.loopOffset = loopOffset;
        root.loops = loops;
        root.cScale = scale;
        grow.restart();
    }

    grow: SequentialAnimation {
        loops: root.loops
        alwaysRunToEnd: true

        PauseAnimation {
            duration: root.offset
        }

        SequentialAnimation {
            PropertyAnimation {
                target: root
                properties: "xScale,yScale"
                from: 1
                to: root.cScale
                duration: 200
            }

            PropertyAnimation {
                target: root
                properties: "xScale,yScale"
                from: root.cScale
                to: 1
                duration: 300
            }

        }

        PauseAnimation {
            duration: root.loopOffset
        }

    }

}
