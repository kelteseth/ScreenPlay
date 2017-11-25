import QtQuick 2.7
import QtQuick.Controls 2.2

Item {
    Connections {
        target: steamWorkshop
        onWorkshopSearched: {
            bannerTxt.text = workshopListModel.getBannerText()
            bannerImg.source = workshopListModel.getBannerUrl()
        }
    }

    Rectangle {
        id: banner
        color: "#131313"
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
            font.pixelSize: 36
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
