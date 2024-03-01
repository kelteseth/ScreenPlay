import QtQuick
import QtQuick.Controls

Pane {
    id: root

    Component.onCompleted: {
        // Add the second handle at 100% after the component is fully created
        sliderHandles.append({"position": sliderLine.width});
    }

    ListModel {
        id: sliderHandles
        ListElement { position: 0 } // Initial handle at 0%
    }

    Rectangle {
        id: sliderLine
        width: parent.width
        height: 10
        anchors.centerIn: parent
        color: "grey"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var newHandlePosition = Math.max(0, Math.min(mouseX, sliderLine.width));
                sliderHandles.append({"position": newHandlePosition});
                // Sort handles after adding a new one
                sortHandles();
            }
        }
    }

    Repeater {
        model: sliderHandles
        delegate: Rectangle {
            id: handle
            width: 10
            height: 20
            x: model.position - width / 2
            y: sliderLine.y - height / 2 + sliderLine.height / 2
            color: "blue"

            MouseArea {
                id: dragArea
                anchors.fill: parent
                drag.target: handle
                drag.axis: Drag.XAxis
                drag.minimumX: getMinimumX(index)
                drag.maximumX: getMaximumX(index)

                onReleased: {
                    sliderHandles.set(index, {"position": handle.x + width / 2});
                }
            }
        }
    }

    function getMinimumX(index) {
        return index > 0 ? sliderHandles.get(index - 1).position : 0;
    }

    function getMaximumX(index) {
        return index < sliderHandles.count - 1 ? sliderHandles.get(index + 1).position - handle.width : sliderLine.width;
    }

    function sortHandles() {
        sliderHandles.sort(function(a, b) {
            return a.position - b.position;
        });
    }

}
