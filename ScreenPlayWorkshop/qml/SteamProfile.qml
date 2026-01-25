import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayWorkshop
import ScreenPlayCore as SPCore

/*!
    \qmltype SteamProfile
    \brief Displays the user's Steam Workshop profile with their published items.

    Shows a grid of the user's workshop items that can be clicked to view
    detailed information and manage the item.
*/
Item {
    id: root
    objectName: "WorkshopProfilePage"

    required property ScreenPlayWorkshop screenPlayWorkshop
    required property SteamWorkshop steamWorkshop
    required property StackView stackView

    StackView.onActivated: root.steamWorkshop.requestUserItems()

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
                        function onAvatarChanged(_avatar: var): void {
                            avatar.setImage(_avatar)
                        }

                        target: root.steamWorkshop.steamAccount
                    }
                }

                Text {
                    text: root.steamWorkshop.steamAccount.username
                    font.pointSize: 12
                    color: "white"
                }

                Button {
                    text: qsTr("Back")
                    onClicked: root.stackView.pop()
                }
            }
        }

        SPCore.MaterialGridView {
            id: gridView
            objectName: "profileGridView"

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

            delegate: Item {
                id: delegateItem

                width: gridView.cellWidth
                height: gridView.cellHeight
                objectName: "profileWorkshopItem" + index

                WorkshopItem {
                    id: workshopItem
                    imgUrl: m_workshopPreview
                    name: m_workshopTitle
                    publishedFileID: m_publishedFileID
                    additionalPreviewUrl: m_additionalPreviewUrl
                    subscriptionCount: m_subscriptionCount
                    itemIndex: index
                    steamWorkshop: root.steamWorkshop

                    anchors {
                        fill: parent
                        margins: 5
                    }

                    onClicked: (publishedFileID, imgUrl) => {
                        root.stackView.push("qrc:/qt/qml/ScreenPlayWorkshop/qml/SteamProfileWorkshopItem.qml", {
                            "stackView": root.stackView,
                            "steamWorkshop": root.steamWorkshop,
                            "publishedFileID": publishedFileID,
                            "previewImageUrl": imgUrl
                        })
                    }
                }
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
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(root.steamWorkshop.workshopProfileListModel.currentPage - 1)
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
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(root.steamWorkshop.workshopProfileListModel.currentPage + 1)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}

