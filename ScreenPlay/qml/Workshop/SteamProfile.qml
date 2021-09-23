import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Workshop 1.0
import ScreenPlay 1.0
import "upload/"
import "../Common" as Common

Item {
    id: root

    property ScreenPlayWorkshop screenPlayWorkshop
    property SteamWorkshop steamWorkshop

    signal requestBack
    Component.onCompleted: steamWorkshop.requestUserItems()

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
                SteamImage {
                    id: avatar

                    width: 70
                    height: 70
                    Component.onCompleted: {
                        root.steamWorkshop.steamAccount.loadAvatar()
                    }

                    Connections {
                        function onAvatarChanged(_avatar) {
                            avatar.setImage(_avatar)
                            avatarPlaceholder.opacity = 0
                        }

                        target: root.steamWorkshop.steamAccount
                    }
                    Image {
                        id: avatarPlaceholder
                        anchors.fill: parent
                        source: "qrc:/assets/icons/steam_default_avatar.png"
                    }
                }

                Text {
                    text: root.steamWorkshop.steamAccount.username
                    font.pointSize: 12
                    color: "white"
                    font.family: ScreenPlay.settings.font
                    font.weight: Font.Thin
                }

                Button {
                    text: qsTr("Back")
                    onClicked: root.requestBack()
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
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(
                                    root.steamWorkshop.workshopProfileListModel.currentPage - 1)
                    }
                }

                Text {
                    id: txtPage

                    Layout.alignment: Qt.AlignVCenter
                    text: root.steamWorkshop.workshopProfileListModel.currentPage
                          + "/" + root.steamWorkshop.workshopProfileListModel.pages
                    font.family: ScreenPlay.settings.font
                    color: Material.primaryTextColor
                }

                Button {
                    id: btnForward

                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Forward")
                    enabled: root.steamWorkshop.workshopProfileListModel.currentPage
                             <= root.steamWorkshop.workshopProfileListModel.pages - 1
                    onClicked: {
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(
                                    root.steamWorkshop.workshopProfileListModel.currentPage + 1)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
