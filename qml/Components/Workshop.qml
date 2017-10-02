import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

CustomPage {
    pageName: ""

    Component.onCompleted: {
        steamWorkshop.searchWorkshop()
    }

    Connections{
        target: steamWorkshop
        onWorkshopSearched:{
            busyIndicator.running = false
            txtBusyIndicator.visible = false
        }

    }

    BusyIndicator {
        id: busyIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        running: true
        Material.accent: Material.Orange
    }

    Text {
        id: txtBusyIndicator
        text: qsTr("Searching for workshop content. Please stand by")
        anchors.top: busyIndicator.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
    }


    GridView {
        id: gridView
        boundsBehavior: Flickable.DragOverBounds
        cacheBuffer: 1000
        maximumFlickVelocity: 10000
        anchors.fill: parent
        cellWidth: 330
        cellHeight: 200
        anchors.margins: 30

        model:  workshopListModel
        delegate:   Rectangle {
            color: "steelblue"
            id:delegate
            width: 180
            height:100
            focus: true
            Image {
                id: img
                anchors.fill: parent
                asynchronous: true
                cache: true
                source: workshopPreview
                smooth: false
            }
            Text {
                id: namea
                anchors.fill: parent
                text: workshopTitle
            }


        }

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
        }
    }

}
