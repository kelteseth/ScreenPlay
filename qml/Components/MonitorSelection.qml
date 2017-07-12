import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    id: rect
    color: "gray"
    // Width of the Sidebar or Space that should be used
    property real availableWidth: 0

    Repeater {
        id: rp
        anchors.fill: parent
        anchors.centerIn: parent
        model: monitorListModel

        Component.onCompleted: {

            //  Absolute availableVirtualGeometry
            var size = monitorListModel.getAbsoluteDesktopSize()

            for (var i = 0; i < rp.count; i++) {
                // Height that is left to draw this monitor
                print("rp.itemAt( i).height:" + rp.itemAt( i).height + "size.height " + size.height +"availableWidth " +availableWidth)
                var availableHeight = ((availableWidth / size.height) * rp.itemAt( i).height)

                // Delta (height/width)
                var monitorHeightRationDelta = 0

                // Scale
                if (rp.itemAt(i).height < rp.itemAt(i).width) {
                    monitorHeightRationDelta = (rp.itemAt(i).height / rp.itemAt(i).width)
                    rp.itemAt(i).height = (monitorHeightRationDelta * availableHeight)
                    print("height mon" + i + " value: " + rp.itemAt(i).height + "delta:" + monitorHeightRationDelta + " availableHeight: " + availableHeight  )
                    rp.itemAt(i).width = (availableWidth / size.width) * rp.itemAt( i).width
                } /*
                else {
                    monitorHeightRationDelta = (rp.itemAt(i).width / rp.itemAt( i).height)
                    rp.itemAt(i).height = availableHeight
                    rp.itemAt(i).width = (availableWidth / size.width) * monitorHeightRationDelta
                }*/

                // Position
                    rp.itemAt(i).x = (availableWidth / size.width) * rp.itemAt( i).x
                    rp.itemAt(i).y = (availableWidth / size.height) * rp.itemAt( i).y
//                rp.itemAt(i).x = (availableWidth / size.width) * rp.itemAt( i).x + availableWidth / 2
//                rp.itemAt(i).y = (availableWidth / size.height) * rp.itemAt( i).y + availableWidth / 2

                print("x: " + rp.itemAt(i).x + "y " + rp.itemAt(i).y)
                print("höhe: " + rp.itemAt(i).height + " breite: " + rp.itemAt(i).width)
            }
        }

        delegate: Rectangle {

            color: "red"
            height: monitorAvailableGeometry.height
            width: monitorAvailableGeometry.width
            x: monitorAvailableGeometry.x
            y: monitorAvailableGeometry.y

            anchors.margins: 10

            Column {
                spacing: 5

                Text {
                    text: monitorNumber
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: monitorName
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: monitorSize.width + " " + monitorSize.height
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: monitorAvailableGeometry.x + " " + monitorAvailableGeometry.y
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
}
