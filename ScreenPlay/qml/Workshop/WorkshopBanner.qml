import QtQuick 2.12
import QtQuick.Controls 2.3

Item {
    Connections {
        target: steamWorkshop
        function onWorkshopSearched() {
            bannerTxt.text = workshopListModel.getBannerText()
            bannerImg.source = workshopListModel.getBannerUrl()
        }
    }

    Rectangle {
        id: banner
        color: "#44131313"
        height: 350
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        Image {
            id: bannerImg
            anchors {
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }

        Text {
            id: bannerTxt
            text: "loading"
            font.pointSize: 36
            color: "white"
        }
    }

    Item {
        id: searchBar
        height: 70
        anchors {
            top: banner.bottom
            right: parent.right
            left: parent.left
        }
    }
}
