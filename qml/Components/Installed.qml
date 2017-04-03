import QtQuick 2.7
import QtQml.Models 2.2



Page {
    id:pageInstalled
    pageName: "Installed"

    signal toggleSidebar(var screenName)


    GridView {
        id: gridView
        anchors.fill: parent
        cellWidth: 320
        cellHeight: 200
        anchors.margins: 30





        delegate: ScreenPlayItem {
            id:delegate
            focus: true
            customTitle: title
            Connections {
                target: delegate
                onItemClicked: {
                    toggleSidebar(screenName)
                }
            }
        }


        model: installedListModel
    }


}
