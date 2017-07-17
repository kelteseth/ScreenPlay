import QtQuick 2.7
import QtGraphicalEffects 1.0

Rectangle {
    id: rect
    color: "gray"
    // Width of the Sidebar or Space that should be used
    property real availableWidth: 0
    property real availableHeight: 0

    Repeater {
        id: rp
        anchors.fill: parent
        anchors.centerIn: parent
        model: monitorListModel

        Component.onCompleted: {

            //  Absolute availableVirtualGeometry
            var absoluteDesktopSize = monitorListModel.getAbsoluteDesktopSize()

            // Delta (height/width)
            var monitorHeightRationDelta = availableHeight / absoluteDesktopSize.height
            var monitorWidthRationDelta =  availableWidth / absoluteDesktopSize.width
            print("DELTAS: " + monitorHeightRationDelta + ", " + monitorWidthRationDelta)

            for (var i = 0; i < rp.count; i++) {


                print(rp.itemAt(i).height + " , " + rp.itemAt(i).width+ " , " +rp.itemAt(i).x+ " , " +rp.itemAt(i).y)
                var newHeight = rp.itemAt(i).height / monitorHeightRationDelta
                rp.itemAt(i).height =  newHeight
                var newWidth = rp.itemAt(i).width / monitorWidthRationDelta
                rp.itemAt(i).width = newWidth
                rp.itemAt(i).x = rp.itemAt(i).x / monitorWidthRationDelta
                rp.itemAt(i).y = rp.itemAt(i).y / monitorHeightRationDelta

                print(rp.itemAt(i).height + " , " + rp.itemAt(i).width+ " , " +rp.itemAt(i).x+ " , " +rp.itemAt(i).y)


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
