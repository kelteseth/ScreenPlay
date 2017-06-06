import QtQuick 2.7
import QtQml.Models 2.2



Page {
    id:pageInstalled
    pageName: ""

    signal setSidebaractiveItem(var screenId)


    GridView {
        id: gridView
        boundsBehavior: Flickable.DragOverBounds
        cacheBuffer: 1000
        maximumFlickVelocity: 10000
        anchors.fill: parent
        cellWidth: 330
        cellHeight: 200
        anchors.margins: 30

        model: installedListModel
        delegate: ScreenPlayItem {
            id:delegate
            focus: true

            customTitle: screenTitle
            screenId: screenFolderId

            Connections {
                target: delegate
                onItemClicked: {
                    setSidebaractiveItem(screenId)
                }
            }
        }

        Component.onCompleted: {

        }

        add: Transition {
                NumberAnimation { properties: "x,y"; from: 100; duration: 1000 }
            }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }




    }


}
