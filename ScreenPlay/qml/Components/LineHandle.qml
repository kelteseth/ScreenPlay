import QtQuick
import QtQuick.Controls.Material
import ScreenPlayApp


Item {
    id: root
    property bool selected: dragHandler.active // User selected
    property bool otherLineHandleActive: false // New property
    property string identifier
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
    signal activated(string identifier, bool active)
    // ⚠️ Note: set the size to 0 so we do not have to
    // handle the offset of the handle width. We set
    // DragHandler margin to 30 to make draggin work.
    width: 0
    height: width

    function toString() {
        console.log(`LineHandle {
        position: ${linePosition.toFixed(6)}
        time: ${timeString}
        selected: ${selected}
        otherLineHandleActive: ${otherLineHandleActive}
        min/max: ${lineMinimum.toFixed(3)}/${lineMaximum.toFixed(3)}
        isLast: ${isLast}
    }`);
    }


    Rectangle {
        id: handleCircle
        visible: !root.isLast
        radius: width
        color: dragHandler.active ? "orange" : "white"
        x: - (width *.5)
        width: 20
        height: width
        opacity: !root.isLast && dragHandler.active ? 1 : 0.25
        Behavior on opacity {
            NumberAnimation{}
        }
    }

    // To block ➕
    MouseArea {
        hoverEnabled: true
        propagateComposedEvents: false
        width: 50
        height: 50
        x: handleCircle.x - (width *.5)
        y: handleCircle.y
    }

    // Text {
    //     id: txt
    //     text: root.lineMinimum + " " + root.lineMaximum + " x:" + root.x
    //     color: Material.secondaryTextColor
    //     opacity: !root.isLast //&& dragHandler.active ? 1 : 0
    //     Behavior on opacity {
    //         NumberAnimation{}
    //     }

    //     anchors {
    //         horizontalCenter: handleCircle.horizontalCenter
    //         top: handleCircle.bottom
    //         topMargin: 30  +  Math.floor(Math.random() * 42);
    //     }
    // }

    DragHandler {
        id: dragHandler
        enabled: !root.isLast && !root.otherLineHandleActive
        target: root
        yAxis.enabled: false
        margin: 30
        xAxis {
            enabled: true
            minimum: root.lineMinimum
            maximum: root.lineMaximum
            onActiveValueChanged: delta => {
                root.handleMoved(root);
            }
        }
        onActiveChanged: {
            root.activated(root.identifier, active);
        }
    }
}
