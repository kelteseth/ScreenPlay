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
            customTitle: title
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
                NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400; }
                NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
            }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }




    }


}
