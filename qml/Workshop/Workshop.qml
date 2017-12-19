import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    id: workshop
    anchors.fill: parent

    //property alias bannerImgProxy: bannerImg2
    signal openCreate

    Component.onCompleted: {
        workshopListModel.clear()
        steamWorkshop.searchWorkshop()
    }

    Connections {
        target: workshopAlertBanner
        onOpenCreate: {
            openCreate()
        }
    }

    GridView {
        id: gridView
        maximumFlickVelocity: 7000
        flickDeceleration: 5000
        anchors.fill: parent
        cellWidth: 330
        cellHeight: 190

        anchors {
            topMargin: 0
            rightMargin: 0
            leftMargin: 30
        }
        onContentYChanged: {
            if (gridView.contentY < 0) {
                workshopScrollBar.active = false
            } else if (!workshopScrollBar.active) {
                workshopScrollBar.active = true
            }
        }

        boundsBehavior: Flickable.StopAtBounds

        header: Item {
            height: 530
            anchors {
                right: parent.right
                left: parent.left
            }

            Connections {
                target: steamWorkshop
                onWorkshopSearched: {
                    bannerImg2.source = workshopListModel.getBannerUrl()
                    bannerTxt.text = workshopListModel.getBannerText()
                }
            }
            Rectangle {
                id: banner
                color: "#131313"
                height: 350
                z: 5
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    leftMargin: -30
                }
                Image {
                    id: bannerImg2
                    anchors {
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }
                    height: {
                        // Calculate parallax scrolling
                        // f(x) = (x * -1)
                        if (gridView.contentY <= 0) {
                            return (gridView.contentY * -1)
                        }
                        return 500
                    }

                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop
                }

                Text {
                    id: bannerTxt
                    text: "loading"
                    font.pixelSize: 36
                    color: "white"
                    width: 400
                    anchors {
                        top: parent.top
                        topMargin: 100
                        left: parent.left
                        leftMargin: 30
                    }
                }

                Button {
                    text: "Download"
                    anchors {
                        top: bannerTxt.bottom
                        topMargin: 100
                        left: parent.left
                        leftMargin: 30
                    }
                    z: 99
                    onClicked: {

                        steamWorkshop.subscribeItem(
                                    workshopListModel.getBannerID())
                    }
                }
            }

            Item {
                id: searchBar
                height: 100
                visible: false
                anchors {
                    top: banner.bottom
                    right: parent.right
                    rightMargin: 60
                    left: parent.left
                    margins: 30
                }

                Rectangle {
                    height: 60
                    width: 400
                    radius: 3
                    anchors {
                        top: parent.top
                        left: parent.left
                        margins: 10
                    }
                    TextField {
                        placeholderText: qsTr("Enter name")
                        anchors.fill: parent
                        anchors.margins: 5
                    }
                }
            }
        }

        model: workshopListModel

        delegate: WorkshopItem {
            imgUrl: workshopPreview
            name: workshopTitle
            steamID: workshopID
        }

        add: Transition {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1.0
                duration: 400
            }
        }
        ScrollBar.vertical: ScrollBar {
            id: workshopScrollBar
            snapMode: ScrollBar.SnapOnRelease
        }
    }

    WorkshopAlertBanner {
        id: workshopAlertBanner
        Component.onCompleted: {
            if (!screenPlaySettings.hasWorkshopBannerSeen) {
                workshopAlertBanner.state = "in"
            }
        }
    }
}
