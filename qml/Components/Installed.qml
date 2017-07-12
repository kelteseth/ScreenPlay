import QtQuick 2.7
import QtQml.Models 2.2



Tab {
    id:pageInstalled
    tabName: ""

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
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
        }
    }


}
