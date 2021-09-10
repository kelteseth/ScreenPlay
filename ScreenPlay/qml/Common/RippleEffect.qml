/*
 * Based on:
 * https://github.com/rschiang/material
 * (THE BSD 2-CLAUSE LICENSE)
 */

import QtQuick 2.0
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material

Item {
    id: root

    property alias radius: mask.radius
    property color color: Material.accent
    property var target
    property int duration: 600

    function trigger() {
        var wave = ripple.createObject(container, {
            "startX": root.width * 0.5,
            "startY": root.height * 0.5,
            "maxRadius": furthestDistance(root.width * 0.5, root.height * 0.5)
        });
    }

    function distance(x1, y1, x2, y2) {
        return Math.sqrt(Math.pow(x1 - x2, 2) + Math.pow(y1 - y2, 2));
    }

    function furthestDistance(x, y) {
        return Math.max(distance(x, y, 0, 0), distance(x, y, width, height), distance(x, y, 0, height), distance(x, y, width, 0));
    }

    anchors.fill: parent

    Rectangle {
        id: mask

        anchors.fill: parent
        color: "black"
        visible: false
    }

    Item {
        id: container

        anchors.fill: parent
        visible: false
    }

    OpacityMask {
        anchors.fill: parent
        source: container
        maskSource: mask
    }

    Component {
        id: ripple

        Rectangle {
            id: ink

            property int startX
            property int startY
            property int maxRadius: 150

            function fadeIfApplicable() {
                if (!fadeAnimation.running)
                    fadeAnimation.start();

            }

            radius: 0
            opacity: 0.25
            color: root.color
            x: startX - radius
            y: startY - radius
            width: radius * 2
            height: radius * 2
            Component.onCompleted: {
                growAnimation.start();
                if (!fadeAnimation.running)
                    fadeAnimation.start();

            }

            NumberAnimation {
                id: growAnimation

                target: ink
                property: "radius"
                from: 0
                to: maxRadius
                duration: 550
                easing.type: Easing.OutCubic
            }

            SequentialAnimation {
                id: fadeAnimation

                NumberAnimation {
                    target: ink
                    property: "opacity"
                    from: 0.8
                    to: 0
                    duration: root.duration
                }

                ScriptAction {
                    script: ink.destroy()
                }

            }

        }

    }

}
