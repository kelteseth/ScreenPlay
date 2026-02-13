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
    detailed information and manage the item. Aggregate statistics (item count,
    total subscriptions) are calculated in C++ by SteamWorkshopListModel.
*/
Item {
    id: root
    objectName: "SteamProfile"

    required property ScreenPlayWorkshop screenPlayWorkshop
    required property SteamWorkshop steamWorkshop
    required property StackView stackView
    required property Background background

    // Only load once on push, not when revealed by popping SteamProfileWorkshopItem
    Component.onCompleted: root.steamWorkshop.requestUserItems()

    function refreshItems(): void {
        root.steamWorkshop.requestUserItems(
            gridView.headerItem.cbListType.currentValue,
            gridView.headerItem.cbSortOrder.currentValue
        )
    }

    focus: true
    Keys.onEscapePressed: root.stackView.pop()
    // Mouse back button is a pointer event, not a key event, so TapHandler is needed
    TapHandler {
        acceptedButtons: Qt.BackButton
        onTapped: root.stackView.pop()
    }

    Image {
        id: backgroundImage
        anchors.fill: parent
        source: root.steamWorkshop.workshopProfileListModel.bannerUrl
        fillMode: Image.PreserveAspectCrop

        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "transparent"
                }
                GradientStop {
                    position: 0.3
                    color: "transparent"
                }
                GradientStop {
                    position: 0.7
                    color: Qt.rgba(Material.backgroundColor.r, Material.backgroundColor.g, Material.backgroundColor.b, 0.85)
                }
                GradientStop {
                    position: 1.0
                    color: Material.backgroundColor
                }
            }
        }
    }

    SPCore.MaterialGridView {
        id: gridView
        objectName: "profileGridView"

        cellWidth: 330
        cellHeight: 190
        model: root.steamWorkshop.workshopProfileListModel
        boundsBehavior: Flickable.StopAtBounds
        maximumFlickVelocity: 3000
        flickDeceleration: 7500

        anchors {
            fill: parent
            leftMargin: 45
        }

        onContentYChanged: {
            gridView.checkLoadMore()
        }

        function checkLoadMore(): void {
            if (!gridView.atYEnd)
                return
            const model = root.steamWorkshop.workshopProfileListModel
            if (!model)
                return
            if (model.hasMore && !model.isLoading)
                root.steamWorkshop.loadNextProfilePage()
        }

        header: Item {
            id: headerItem

            property alias cbListType: cbListType
            property alias cbSortOrder: cbSortOrder

            height: 90 + 20 // navigation bar + top margin
            width: gridView.width - gridView.anchors.leftMargin

            SPCore.ImageBlurContainer {
                id: headerBar
                backgroundSource: backgroundImage
                flickable: gridView
                stackView: root.stackView
                width: parent.width
                height: 70
                radius: 8
                clip: true

                anchors {
                    top: parent.top
                    topMargin: 20
                    left: parent.left
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 12

                    Button {
                        id: backButton
                        Layout.preferredWidth: implicitWidth
                        icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_arrow_left.svg"
                        flat: true
                        onClicked: root.stackView.pop()
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Back to Workshop")
                    }

                    SteamImage {
                        id: avatar
                        Layout.preferredWidth: 55
                        Layout.preferredHeight: 55
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

                    ColumnLayout {
                        spacing: 0

                        Label {
                            text: root.steamWorkshop.steamAccount.username
                            font.pointSize: 14
                            font.bold: true
                            color: "white"
                        }

                        Label {
                            text: root.steamWorkshop.userTotalSubscriptions.toLocaleString() + " " + qsTr("Subscribers")
                            font.pointSize: 11
                            color: Qt.rgba(1, 1, 1, 0.7)
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    ComboBox {
                        id: cbListType
                        Layout.preferredWidth: 200
                        Layout.alignment: Qt.AlignVCenter
                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 0
                        model: [
                            {
                                "value": SPCore.Steam.EUserUGCList.K_EUserUGCList_Published,
                                "text": qsTr("Your Uploads")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCList.K_EUserUGCList_Subscribed,
                                "text": qsTr("Subscribed")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCList.K_EUserUGCList_Favorited,
                                "text": qsTr("Favorites")
                            }
                        ]
                        onActivated: root.refreshItems()
                    }

                    ComboBox {
                        id: cbSortOrder
                        Layout.preferredWidth: 200
                        Layout.alignment: Qt.AlignVCenter
                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 0
                        model: [
                            {
                                "value": SPCore.Steam.EUserUGCListSortOrder.K_EUserUGCListSortOrder_LastUpdatedDesc,
                                "text": qsTr("Last Updated")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCListSortOrder.K_EUserUGCListSortOrder_CreationOrderDesc,
                                "text": qsTr("Newest First")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCListSortOrder.K_EUserUGCListSortOrder_CreationOrderAsc,
                                "text": qsTr("Oldest First")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCListSortOrder.K_EUserUGCListSortOrder_TitleAsc,
                                "text": qsTr("Title A-Z")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCListSortOrder.K_EUserUGCListSortOrder_SubscriptionDateDesc,
                                "text": qsTr("Subscription Date")
                            },
                            {
                                "value": SPCore.Steam.EUserUGCListSortOrder.K_EUserUGCListSortOrder_VoteScoreDesc,
                                "text": qsTr("Vote Score")
                            }
                        ]
                        onActivated: root.refreshItems()
                    }
                }
            }

            // Empty state — shown when the query finished with no results.
            ColumnLayout {
                anchors {
                    top: headerBar.bottom
                    topMargin: 80
                    horizontalCenter: parent.horizontalCenter
                }
                spacing: 12
                visible: gridView.count === 0
                         && !root.steamWorkshop.workshopProfileListModel.isLoading

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "¯\\_(ツ)_/¯"
                    font.pointSize: 32
                    color: Material.secondaryTextColor
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Nothing here but tumbleweeds and broken dreams.")
                    font.pointSize: 14
                    color: Material.secondaryTextColor
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Try a different list or go create something amazing!")
                    font.pointSize: 11
                    color: Qt.rgba(Material.secondaryTextColor.r,
                                   Material.secondaryTextColor.g,
                                   Material.secondaryTextColor.b, 0.6)
                }
            }
        }

        delegate: WorkshopItem {
            id: workshopItem
            objectName: "profileWorkshopItem" + index
            imgUrl: m_workshopPreview
            name: m_workshopTitle
            publishedFileID: m_publishedFileID
            additionalPreviewUrl: m_additionalPreviewUrl
            subscriptionCount: m_subscriptionCount
            itemIndex: index
            steamWorkshop: root.steamWorkshop

            onClicked: (publishedFileID, imgUrl) => {
                if (m_isOwnItem) {
                    root.stackView.push("qrc:/qt/qml/ScreenPlayWorkshop/qml/SteamProfileWorkshopItem.qml", {
                        "stackView": root.stackView,
                        "steamWorkshop": root.steamWorkshop,
                        "publishedFileID": publishedFileID,
                        "previewImageUrl": imgUrl
                    })
                } else {
                    sidebar.setWorkshopItem(publishedFileID, imgUrl,
                                            m_additionalPreviewUrl, m_subscriptionCount)
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }

        footer: Item {
            width: parent.width
            height: footerContent.implicitHeight + 40

            ColumnLayout {
                id: footerContent
                anchors.centerIn: parent
                spacing: 10

                BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    running: root.steamWorkshop.workshopProfileListModel.isLoading
                    visible: running
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Loading more...")
                    color: Material.secondaryTextColor
                    visible: root.steamWorkshop.workshopProfileListModel.isLoading
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("That's everything! You've reached the end of the internet... well, your corner of it.")
                    color: Material.secondaryTextColor
                    visible: !root.steamWorkshop.workshopProfileListModel.hasMore
                             && !root.steamWorkshop.workshopProfileListModel.isLoading
                             && root.steamWorkshop.workshopProfileListModel.currentPage > 1
                }
            }
        }
    }

    Sidebar {
        id: sidebar

        topMargin: 60
        steamWorkshop: root.steamWorkshop
        onUnsubscribed: publishedFileID => {
            root.steamWorkshop.workshopProfileListModel.removeByPublishedFileID(publishedFileID)
        }
    }
}