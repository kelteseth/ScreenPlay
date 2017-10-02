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
            bannerTxt.text = workshopListModel.getBannerText()
            bannerImg.source = workshopListModel.getBannerUrl();
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

    Rectangle {
        id: banner
        color: "#131313"
        height: 250
        anchors {
            top: parent.top
            right: parent.right
            left:parent.left
        }
        Image {
            id: bannerImg
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

        }

        Text {
            id: bannerTxt
            text: "loading"
            font.pixelSize: 36
            color: "white"
        }
    }

    Item {
        id: searchBar
        height:70
        anchors {
            top: banner.bottom
            right: parent.right
            left:parent.left
        }
    }

    Item {
        anchors {
            top: searchBar.bottom
            right: parent.right
            bottom: parent.bottom
            left:parent.left
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
                    color:"white"
                }


            }

            add: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
            }
        }
    }



}
