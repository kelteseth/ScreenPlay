import QtQuick 2.7
import QtQuick.Controls 2.2

CustomPage {
    id: workshop
    pageName: ""

    Component.onCompleted: {
        steamWorkshop.searchWorkshop()
    }

    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        GridView {
            id: gridView
            cacheBuffer: 1000
            maximumFlickVelocity: 5000
            anchors.fill: parent
            cellWidth: 330
            cellHeight: 200

            boundsBehavior: Flickable.StopAtBounds
            //boundsBehavior: Flickable.OvershootBounds
            header: Item {
                height: 500
                anchors {
                    right: parent.right
                    left: parent.left
                }
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
                        height: {
                            // Calculate parallax scrolling
                            // f(x) = (x * -1)
                            if(gridView.contentY <= 0){
                                return (gridView.contentY * -1)
                            }
                            return 500
                        }

                        onHeightChanged: {
                            print(bannerImg.height, gridView.contentY)
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

            model: workshopListModel

            delegate: WorkshopItem {
                imgUrl: workshopPreview
                name: workshopTitle
            }

            add: Transition {
                NumberAnimation {
                    property: "opacity"
                    from: 0
                    to: 1.0
                    duration: 400
                }
            }
        }
    }
}
