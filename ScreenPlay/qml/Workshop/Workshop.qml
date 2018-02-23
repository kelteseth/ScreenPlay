import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2

Item {
    id: workshop
    anchors.fill: parent
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
            height: 440
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
                    font.pixelSize: 42
                    color: "white"
                    font.family: "Roboto"
                    font.weight: Font.Thin
                    width: 400
                    anchors {
                        top: parent.top
                        topMargin: 100
                        left: parent.left
                        leftMargin: 50
                    }
                }

                Button {
                    text: qsTr("Download now!")
                    Material.background: Material.Orange
                    Material.foreground: "white"
                    icon.source: "qrc:/assets/icons/icon_download.svg"
                    icon.width: 16
                    icon.height: 16
                    anchors {
                        top: bannerTxt.bottom
                        topMargin: 50
                        left: parent.left
                        leftMargin: 50
                    }
                    z: 99
                    onClicked: {
                        text = qsTr("Downloading...")
                        steamWorkshop.subscribeItem(
                                    workshopListModel.getBannerID())
                    }
                }
            }

            Item {
                id: searchBar
                height: 50
                anchors {
                    top: banner.bottom
                    topMargin: 20
                    right: parent.right
                    left: parent.left
                    leftMargin: 20
                }

                Text {
                    id: txtCategory
                    text: qsTr("Trendig Today")
                    color: "#818181"
                    font.pointSize: 18
                    verticalAlignment: Text.AlignVCenter
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
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
