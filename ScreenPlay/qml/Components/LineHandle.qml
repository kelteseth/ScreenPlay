import QtQuick
import QtQuick.Controls
import ScreenPlayApp

Item {
    id: root
    property real lineWidth: 1
    property real linePosition: Number((root.x / lineWidth).toFixed(6))
    property string timeString: {
        const normalized = root.x / root.lineWidth;
        return App.util.getTimeString(normalized);
    }

    property real lineMinimum: .5
    property real lineMaximum: .5
    property bool isLast: false
    property alias dragHandler: dragHandler

    signal handleMoved(var handle)
    signal removeHandle(var handle)
    width: 20
    height: width
    Rectangle {
        id: handleCircle
        visible: !root.isLast
        radius: width
        color: dragHandler.active ? "orange" : "white"
        width: 20
        height: width
    }

    // To block ➕
    MouseArea {
        hoverEnabled: true
        propagateComposedEvents: false
        width: 50
        height: 50
    }

    Text {
        id: txt
        text: root.timeString
        color: "white"
        visible: !root.isLast
        anchors {
            horizontalCenter: handleCircle.horizontalCenter
            bottom: parent.bottom
            bottomMargin: -20
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
                root.handleMoved(root);
            }
        }
    }
}
