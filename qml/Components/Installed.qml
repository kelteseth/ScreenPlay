import QtQuick 2.7
import QtQml.Models 2.2


Page {
    id:pageInstalled
    pageName: "Installed"

    GridView {
        id: gridView
        anchors.fill: parent
        cellWidth: 320
        cellHeight: 200
        anchors.margins: 20
        focus: true

        delegate: ScreenPlayItem {

            customTitle: title
        }


        model: installedListModel
    }


}
