import QtQuick
import QtQuick.Controls

Item {
    id: root
    property real lineWidth: 100
    property real linePosition: (root.x / lineWidth).toFixed(2)
    property real lineMinimum: .5
    property real lineMaximum: .5
    property bool isLast: false
    property alias dragHandler: dragHandler

    signal handleMoved(var handle)
    signal removeHandle(var handle)
    signal updateNeighbors(var handle)
    width: 20
    height: width
    Rectangle {
        visible: !root.isLast
        radius: width
        color: dragHandler.active ? "orange" : "white"
        anchors.fill: parent
    }

    Text {
        id: txt
        //text: root.linePosition + "-"+ (root.linePosition * root.lineWidth).toFixed(2) + "\n" + Math.floor( dragHandler.xAxis.minimum) + " - "+ Math.floor(dragHandler.xAxis.maximum)
        text: {
            const normalized = root.x / root.lineWidth; // Your existing normalization
            let totalHours = normalized * 24;
            let hours = Math.floor(totalHours); // Gets the whole hour part
            let minutes = Math.round((totalHours - hours) * 60); // Calculates the minutes
            // Check if minutes rolled over to 60, adjust hours and minutes accordingly
            if (minutes === 60) {
                hours += 1; // Increment hours by 1
                minutes = 0; // Reset minutes to 0
            }

            // Ensure hours wrap correctly at 24
            if (hours === 24) {
                hours = 0;
            }

            return hours + ":"  + minutes.toString().padStart(2, '0')
        }

        color: "white"
        visible: !root.isLast
        anchors{
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: -20
        }
    }

    DragHandler {
        id: dragHandler
        enabled: !root.isLast
        target: root
        yAxis.enabled: false
        xAxis {
            enabled: true
            minimum: root.lineMinimum
            maximum: root.lineMaximum
            onActiveValueChanged: delta => {
                                      root.handleMoved(root)
                                  }
        }
    }
}
