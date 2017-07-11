import QtQuick 2.0

MouseArea {
    id: mouseArea
    anchors.rightMargin: -11
    anchors.bottomMargin: 282
    anchors.leftMargin: 26
    anchors.topMargin: 486
    anchors.fill: parent

    Rectangle {
        id: rect
        color: "#dfdfdf"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -158
        anchors.right: parent.right
        anchors.rightMargin: 42
        anchors.left: parent.left
        anchors.leftMargin: -7
        anchors.top: parent.top
        anchors.topMargin: 20

        Item {
            id:monitorListCenter
            width:0
            height:0
            anchors.centerIn: parent
        }




        Repeater {
            id:rp
            anchors.fill: parent
            anchors.centerIn: parent
            anchors.margins: 30
            model:monitorListModel

            Component.onCompleted: {

                //  Absolute availableVirtualGeometry
                var size = monitorListModel.getAbsoluteDesktopSize();

                // Width of the Sidebar or Space that should be used
                var sidebarSize = 200

                for(var i = 0; i < rp.count; i++)
                {
                    // Height that is left to draw this monitor
                    var availableHeight = ((sidebarSize / size.height) * rp.itemAt(i).height)

                    // Delta (height/width)
                    var monitorHeightRationDelta = 0;

                    // Scale
                    if(rp.itemAt(i).height < rp.itemAt(i).width )
                    {
                        monitorHeightRationDelta = ( rp.itemAt(i).height / rp.itemAt(i).width)
                        rp.itemAt(i).height = monitorHeightRationDelta * availableHeight
                        rp.itemAt(i).width = (sidebarSize / size.width) * rp.itemAt(i).width
                    }
                    else
                    {
                        monitorHeightRationDelta = ( rp.itemAt(i).width / rp.itemAt(i).height)
                        rp.itemAt(i).height = availableHeight
                        rp.itemAt(i).width = (sidebarSize / size.width) * monitorHeightRationDelta
                    }

                    // Position
                    rp.itemAt(i).x = (sidebarSize / size.width) * rp.itemAt(i).x + sidebarSize/2
                    rp.itemAt(i).y = (sidebarSize / size.height) * rp.itemAt(i).y + sidebarSize/2
                }
            }

            delegate: Rectangle {

                color:"red"
                height: monitorAvailableGeometry.height
                width: monitorAvailableGeometry.width
                x: monitorAvailableGeometry.x
                y: monitorAvailableGeometry.y
//                height: (150 / monitorAvailableVirtualGeometry.height) * monitorAvailableGeometry.height
//                width: (150 / monitorAvailableVirtualGeometry.width) * monitorAvailableGeometry.width
//                x: (150 / monitorAvailableVirtualGeometry.width) * monitorAvailableGeometry.x
//                y: (150 / monitorAvailableVirtualGeometry.height) * monitorAvailableGeometry.y

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
}
