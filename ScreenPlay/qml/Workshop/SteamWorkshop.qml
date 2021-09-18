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

    property ScreenPlayWorkshop workshop
    property SteamWorkshop steam

    signal openSteamProfile

    onVisibleChanged: {
        if (!visible)
            sidebar.close()
    }
    Component.onCompleted: {
        if (root.steam.online) {
            root.steam.searchWorkshop(SteamEnums.K_EUGCQuery_RankedByTrend)
        } else {

            popupOffline.open()
        }
    }

    MouseArea {
        enabled: gridView.count === 0
        z: enabled ? 10 : 0
        cursorShape: enabled ? Qt.WaitCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
        anchors.fill: parent
        preventStealing: true
    }

    Connections {
        function onWorkshopSearched() {
            bannerTxt.text = root.steam.workshopListModel.getBannerText()
            background.backgroundImage = root.steam.workshopListModel.getBannerUrl()
            banner.bannerPublishedFileID = root.steam.workshopListModel.getBannerID()
            bannerTxtUnderline.numberSubscriber
                    = root.steam.workshopListModel.getBannerAmountSubscriber()
        }

        target: root.steam
    }

    Background {
        id: background
        anchors.fill: parent
    }

    UploadProject {
        id: popupUploadProject
        screenPlayWorkshop: workshop
    }

    PopupSteamWorkshopAgreement {
        id: popupSteamWorkshopAgreement
    }

    Connections {
        function onUserNeedsToAcceptWorkshopLegalAgreement() {
            popupSteamWorkshopAgreement.open()
        }

        target: root.steam.uploadListModel
    }

    Flickable {
        id: scrollView

        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: gridView.height + header.height + 150
        onContentYChanged: {
            // Calculate parallax scrolling
            if (contentY >= 0)
                background.imageOffsetTop = (contentY * -0.4)
            else
                background.imageOffsetTop = 0
        }

        Item {
            id: header

            height: 450

            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            Item {
                id: banner

                property var bannerPublishedFileID

                height: header.height

                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }

                Image {
                    id: bannerImg2

                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop

                    anchors {
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }
                }

                ColumnLayout {
                    anchors {
                        top: parent.top
                        topMargin: 100
                        right: parent.right
                        left: parent.left
                        leftMargin: 100
                    }

                    Text {
                        id: bannerTxtUnderline

                        property int numberSubscriber: 0

                        text: numberSubscriber + " SUBSCRIBED TO:"
                        font.pointSize: 12
                        color: "white"
                        font.family: ScreenPlay.settings.font
                        font.weight: Font.Thin
                    }

                    Text {
                        id: bannerTxt

                        text: qsTr("Loading")
                        font.pointSize: 42
                        color: "white"
                        font.family: ScreenPlay.settings.font
                        font.weight: Font.Thin
                        width: 400
                    }

                    RowLayout {
                        spacing: 10

                        Button {
                            text: qsTr("Download now!")
                            Material.background: Material.accent
                            Material.foreground: "white"
                            icon.source: "qrc:/assets/icons/icon_download.svg"
                            onClicked: {
                                text = qsTr("Downloading...")
                                root.steam.subscribeItem(
                                            root.steam.workshopListModel.getBannerID(
                                                ))
                            }
                        }

                        Button {
                            text: qsTr("Details")
                            Material.background: Material.accent
                            Material.foreground: "white"
                            icon.source: "qrc:/assets/icons/icon_info.svg"
                            visible: false
                            onClicked: {
                                sidebar.setWorkshopItem(publishedFileID,
                                                        imgUrl,
                                                        additionalPreviewUrl,
                                                        subscriptionCount)
                            }
                        }
                    }

                    MouseArea {
                        onClicked: Qt.openUrlExternally(
                                       "steam://url/CommunityFilePage/"
                                       + banner.bannerPublishedFileID)
                        height: 30
                        width: bannerTxtOpenInSteam.paintedWidth
                        cursorShape: Qt.PointingHandCursor

                        Text {
                            id: bannerTxtOpenInSteam

                            opacity: 0.7
                            text: qsTr("Open In Steam")
                            font.pointSize: 10
                            color: "white"
                            font.underline: true
                            font.family: ScreenPlay.settings.font
                            font.weight: Font.Thin
                        }
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
            model: root.steam.workshopListModel
            boundsBehavior: Flickable.StopAtBounds

            anchors {
                top: header.bottom
                topMargin: 100
                left: parent.left
                right: parent.right
                leftMargin: 45
            }

            header: Item {
                property alias searchField: tiSearch

                height: 80
                width: gridView.width - gridView.anchors.leftMargin

                Rectangle {
                    color: Material.backgroundColor
                    radius: 3
                    width: parent.width - 20
                    height: 70
                    anchors.centerIn: parent

                    SteamImage {
                        id: avatar

                        width: 70
                        height: 70
                        Component.onCompleted: {
                            steam.steamAccount.loadAvatar()
                        }

                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        Connections {
                            function onAvatarChanged(_avatar) {
                                avatar.setImage(_avatar)
                                avatarPlaceholder.opacity = 0
                            }

                            target: steam.steamAccount
                        }
                    }

                    Image {
                        id: avatarPlaceholder
                        anchors.fill: avatar
                        source: "qrc:/assets/icons/steam_default_avatar.png"
                    }

                    Button {
                        id: btnSteamProfile

                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: avatar.right
                            leftMargin: 20
                        }

                        text: qsTr("Profile")
                        onClicked: root.openSteamProfile()
                    }

                    Button {
                        id: btnSteamUpload

                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: btnSteamProfile.right
                            leftMargin: 20
                        }

                        text: qsTr("Upload")
                        onClicked: popupUploadProject.open()
                    }

                    Item {
                        id: searchWrapper

                        width: 400
                        height: 50

                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: btnSteamUpload.right
                            leftMargin: 20
                        }

                        TextField {
                            id: tiSearch
                            placeholderText: qsTr("Search for Wallpaper and Widgets...")
                            onEditingFinished: root.steam.searchWorkshopByText(
                                                   tiSearch.text)
                            anchors {
                                top: parent.top
                                right: parent.right
                                bottom: parent.bottom
                                left: parent.left
                                leftMargin: 10
                            }
                        }

                        ToolButton {
                            id: tb
                            icon.source: "qrc:/assets/icons/icon_search.svg"
                            onClicked: root.steam.searchWorkshopByText(
                                           tiSearch.text)
                            icon.width: 20
                            icon.height: 20
                            anchors {
                                right: parent.right
                                bottom: parent.bottom
                                bottomMargin: 10
                            }
                        }
                    }

                    RowLayout {
                        spacing: 20

                        anchors {
                            left: searchWrapper.right
                            leftMargin: 20
                            right: cbQuerySort.left
                            rightMargin: 20
                            verticalCenter: parent.verticalCenter
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        Button {
                            text: qsTr("Open Workshop in Steam")
                            font.capitalization: Font.Capitalize
                            font.family: ScreenPlay.settings.font
                            onClicked: Qt.openUrlExternally(
                                           "steam://url/SteamWorkshopPage/672870")
                            icon.source: "qrc:/assets/icons/icon_steam.svg"
                            icon.width: 18
                            icon.height: 18
                            height: cbQuerySort.height
                        }
                    }

                    ComboBox {
                        id: cbQuerySort

                        width: 220
                        height: searchWrapper.height
                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 2
                        Layout.preferredHeight: searchWrapper.height
                        font.family: ScreenPlay.settings.font
                        model: [{
                                "value": SteamEnums.k_EUGCQuery_RankedByVote,
                                "text": qsTr("Ranked By Vote")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_RankedByPublicationDate,
                                "text": qsTr("Publication Date")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_RankedByTrend,
                                "text": qsTr("Ranked By Trend")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_FavoritedByFriendsRankedByPublicationDate,
                                "text": qsTr("Favorited By Friends")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_CreatedByFriendsRankedByPublicationDate,
                                "text": qsTr("Created By Friends")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate,
                                "text": qsTr("Created By Followed Users")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_NotYetRated,
                                "text": qsTr("Not Yet Rated")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_RankedByTotalVotesAsc,
                                "text": qsTr("Total VotesAsc")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_RankedByVotesUp,
                                "text": qsTr("Votes Up")
                            }, {
                                "value": SteamEnums.K_EUGCQuery_RankedByTotalUniqueSubscriptions,
                                "text": qsTr("Total Unique Subscriptions")
                            }]
                        onActivated: {
                            root.steam.searchWorkshop(cbQuerySort.currentValue)
                        }

                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: 10
                        }
                    }
                }
            }

            delegate: WorkshopItem {
                imgUrl: m_workshopPreview
                name: m_workshopTitle
                publishedFileID: m_publishedFileID
                additionalPreviewUrl: m_additionalPreviewUrl
                subscriptionCount: m_subscriptionCount
                itemIndex: index
                steamWorkshop: root.steam
                onClicked: {
                    sidebar.setWorkshopItem(publishedFileID, imgUrl,
                                            additionalPreviewUrl,
                                            subscriptionCount)
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
                    enabled: root.steam.workshopListModel.currentPage > 1
                    onClicked: {
                        root.steam.workshopListModel.setCurrentPage(
                                    root.steam.workshopListModel.currentPage - 1)
                        root.steam.searchWorkshop(
                                    SteamEnums.K_EUGCQuery_RankedByTrend)
                    }
                }

                Text {
                    id: txtPage

                    Layout.alignment: Qt.AlignVCenter
                    text: root.steam.workshopListModel.currentPage + "/"
                          + root.steam.workshopListModel.pages
                    font.family: ScreenPlay.settings.font
                    color: Material.primaryTextColor
                }

                Button {
                    id: btnForward

                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Forward")
                    enabled: root.steam.workshopListModel.currentPage
                             <= root.steam.workshopListModel.pages - 1
                    onClicked: {
                        root.steam.workshopListModel.setCurrentPage(
                                    root.steam.workshopListModel.currentPage + 1)
                        root.steam.searchWorkshop(
                                    SteamEnums.K_EUGCQuery_RankedByTrend)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Behavior on contentHeight {
            PropertyAnimation {
                duration: 400
                property: "contentHeight"
                easing.type: Easing.InOutQuart
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }

    Sidebar {
        id: sidebar

        topMargin: 60
        steamWorkshop: root.steam
        onTagClicked: {
            gridView.headerItem.searchField.text = tag
            sidebar.close()
        }
    }
}
