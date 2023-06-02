import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import ScreenPlayWorkshop
import "upload/"

Item {
    id: root
    objectName: "WorkshopProfilePage"

    property ScreenPlayWorkshop screenPlayWorkshop
    property SteamWorkshop steamWorkshop
    property StackView stackView
    StackView.onActivated: steamWorkshop.requestUserItems()

    Flickable {
        id: scrollView

        anchors.fill: parent
        contentWidth: root.width
        contentHeight: gridView.height + header.height + 150

        Item {
            id: header
            height: 200
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: 45
            }

            RowLayout {
                anchors {
                    left: parent.left
                    leftMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                spacing: 20
                SteamProfilePicture {
                    id: avatar

                    width: 70
                    height: 70
                }

                Text {
                    text: root.steamWorkshop.steamAccount.username
                    font.pointSize: 12
                    color: "white"
                }

                Button {
                    text: qsTr("Back")
                    onClicked: {
                        stackView.pop();
                    }
                }
            }
        }

        GridView {
            id: gridView

            maximumFlickVelocity: 7000
            flickDeceleration: 5000
            cellWidth: 330
            cellHeight: 190
            height: contentHeight
            interactive: false
            model: root.steamWorkshop.workshopProfileListModel
            boundsBehavior: Flickable.StopAtBounds

            anchors {
                top: header.bottom
                topMargin: 40
                left: parent.left
                right: parent.right
                leftMargin: 45
            }

            delegate: WorkshopItem {
                imgUrl: m_workshopPreview
                name: m_workshopTitle
                publishedFileID: m_publishedFileID
                additionalPreviewUrl: m_additionalPreviewUrl
                subscriptionCount: m_subscriptionCount
                itemIndex: index
                steamWorkshop: root.steamWorkshop
                //            onClicked: {
                //                sidebar.setWorkshopItem(publishedFileID, imgUrl,
                //                                        additionalPreviewUrl,
                //                                        subscriptionCount)
                //            }
            }

            ScrollBar.vertical: ScrollBar {
                id: workshopScrollBar

                snapMode: ScrollBar.SnapOnRelease
            }

            footer: RowLayout {
                height: 150
                width: parent.width
                spacing: 10

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: btnBack

                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Back")
                    enabled: root.steamWorkshop.workshopProfileListModel.currentPage > 1
                    onClicked: {
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(root.steamWorkshop.workshopProfileListModel.currentPage - 1);
                    }
                }

                Text {
                    id: txtPage

                    Layout.alignment: Qt.AlignVCenter
                    text: root.steamWorkshop.workshopProfileListModel.currentPage + "/" + root.steamWorkshop.workshopProfileListModel.pages
                    color: Material.primaryTextColor
                }

                Button {
                    id: btnForward

                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Forward")
                    enabled: root.steamWorkshop.workshopProfileListModel.currentPage <= root.steamWorkshop.workshopProfileListModel.pages - 1
                    onClicked: {
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(root.steamWorkshop.workshopProfileListModel.currentPage + 1);
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
