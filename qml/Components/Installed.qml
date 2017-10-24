import QtQuick 2.7
import QtQml.Models 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4

CustomPage {
    id:pageInstalled
    pageName: ""

    signal setSidebaractiveItem(var screenId)


    GridView {
        id: gridView
        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 7000
        flickDeceleration: 5000
        anchors.fill: parent
        cellWidth: 330
        cellHeight: 200
        anchors {
            topMargin: 0
            rightMargin:0
            leftMargin: 30
        }
        header: Item {
            height:10
            width: parent.width
        }
        model:installedListModel

        delegate: ScreenPlayItem {
            id:delegate
            focus: true

            customTitle: screenTitle
            screenId: screenFolderId
            absoluteStoragePath: screenAbsoluteStoragePath

            Connections {
                target: delegate
                onItemClicked: {
                    setSidebaractiveItem(screenId)
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            stepSize: 100
            snapMode: ScrollBar.SnapOnRelease


        }
    }


}
