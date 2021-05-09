import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12

import ScreenPlay.Workshop 1.0
import ScreenPlay.Workshop.SteamEnums 1.0
import ScreenPlay 1.0

import "upload/"

import "../Common" as Common

Item {
    id: root
    state: "base"
    onVisibleChanged: {
        if (!visible)
            sidebar.close()
    }

    property alias steamWorkshop: screenPlayWorkshop.steamWorkshop

    MouseArea {
        enabled: gridView.count === 0
        z: enabled ? 10 : 0
        cursorShape: enabled ? Qt.WaitCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
        anchors.fill: parent
        preventStealing: true
    }

    ScreenPlayWorkshop {
        id: screenPlayWorkshop
    }

    Component.onCompleted: {
        if (steamWorkshop.online) {
            steamWorkshop.searchWorkshop(SteamEnums.K_EUGCQuery_RankedByTrend)
        } else {
            popupOffline.open()
        }
    }

    Connections {
        target: steamWorkshop
        function onWorkshopSearched() {
            bannerTxt.text = steamWorkshop.workshopListModel.getBannerText()
            background.backgroundImage = steamWorkshop.workshopListModel.getBannerUrl()
            banner.bannerPublishedFileID = steamWorkshop.workshopListModel.getBannerID()
            bannerTxtUnderline.numberSubscriber
                    = steamWorkshop.workshopListModel.getBannerAmountSubscriber(
                        )
        }
    }

    Background {
        id: background
        anchors.fill: parent
    }

    PopupOffline {
        id: popupOffline
    }

    UploadProject {
        id: popupUploadProject
        steamWorkshop: root.steamWorkshop
        workshop: screenPlayWorkshop
    }

    PopupSteamWorkshopAgreement {
        id: popupSteamWorkshopAgreement
    }

    Connections {
        target: steamWorkshop.uploadListModel
        function onUserNeedsToAcceptWorkshopLegalAgreement() {
            popupSteamWorkshopAgreement.open()
        }
    }

    Navigation {
        id: nav
        steamWorkshop: root.steamWorkshop
        z: 3
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        onUploadPressed: popupUploadProject.open()
    }

    Flickable {
        id: scrollView
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: gridView.height + header.height + 150

        Behavior on contentHeight {
            PropertyAnimation {
                duration: 400
                property: "contentHeight"
                easing.type: Easing.InOutQuart
            }
        }

        onContentYChanged: {
            // Calculate parallax scrolling
            if (contentY >= 0) {
                background.imageOffsetTop = (contentY * -.4)
            } else {
                background.imageOffsetTop = 0
            }
            if (contentY >= (header.height)) {
                root.state = "scrolling"
            } else {
                root.state = "base"
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }

        Item {
            id: header
            height: 350
            anchors {
                top: parent.top
                topMargin: nav.height
                right: parent.right
                left: parent.left
            }

            Item {
                id: banner
                height: 350
                property var bannerPublishedFileID
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }
                Image {
                    id: bannerImg2
                    anchors {
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }

                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop
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
                                steamWorkshop.subscribeItem(
                                            steamWorkshop.workshopListModel.getBannerID(
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
                            opacity: .7
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

            model: steamWorkshop.workshopListModel
            anchors {
                top: header.bottom
                topMargin: 100
                left: parent.left
                right: parent.right
                leftMargin: 45
            }

            header: Item {
                height: 80
                width: gridView.width - gridView.anchors.leftMargin
                property alias searchField: tiSearch

                Rectangle {
                    color: Material.backgroundColor
                    radius: 3
                    width: parent.width - 10
                    height: 70
                    anchors.centerIn: parent

                    Item {
                        id: searchWrapper
                        width: 400
                        height: 50
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            leftMargin: 10
                        }

                        TextField {
                            id: tiSearch
                            anchors {
                                top: parent.top
                                right: parent.right
                                bottom: parent.bottom
                                left: parent.left
                                leftMargin: 10
                            }
                            leftPadding: 25
                            selectByMouse: true
                            placeholderText: qsTr("Search for Wallpaper and Widgets...")
                            onTextChanged: timerSearch.restart()
                            Timer {
                                id: timerSearch
                                interval: 500
                                onTriggered: steamWorkshop.searchWorkshopByText(
                                                 tiSearch.text)
                            }
                        }

                        Image {
                            source: "qrc:/assets/icons/icon_search.svg"
                            width: 14
                            height: width
                            sourceSize: Qt.size(width, width)
                            anchors {
                                left: parent.left
                                leftMargin: 15
                                bottom: parent.bottom
                                bottomMargin: 22
                            }
                        }

                        ToolButton {
                            id: tb
                            enabled: tiSearch.text !== ""
                            anchors {
                                right: parent.right
                                bottom: parent.bottom
                                bottomMargin: 10
                            }
                            icon.source: "qrc:/assets/icons/font-awsome/close.svg"

                            onClicked: tiSearch.text = ""
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
                        Button {
                            text: qsTr("Open GameHub in Steam")
                            font.capitalization: Font.Capitalize
                            font.family: ScreenPlay.settings.font
                            onClicked: Qt.openUrlExternally(
                                           "steam://url/GameHub/672870")
                            icon.source: "qrc:/assets/icons/icon_steam.svg"
                            icon.width: 18
                            icon.height: 18
                            height: cbQuerySort.height
                        }
                    }

                    ComboBox {
                        id: cbQuerySort
                        width: 250
                        height: searchWrapper.height
                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: 10
                        }

                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 2
                        Layout.preferredHeight: searchWrapper.height
                        font.family: ScreenPlay.settings.font
                        onActivated: {
                            steamWorkshop.searchWorkshop(
                                        cbQuerySort.currentValue)
                        }
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
                    }
                }
            }

            boundsBehavior: Flickable.StopAtBounds

            delegate: WorkshopItem {
                imgUrl: m_workshopPreview
                name: m_workshopTitle
                publishedFileID: m_publishedFileID
                additionalPreviewUrl: m_additionalPreviewUrl
                subscriptionCount: m_subscriptionCount
                itemIndex: index
                steamWorkshop: root.steamWorkshop
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
                    enabled: steamWorkshop.workshopListModel.currentPage > 1
                    onClicked: {
                        steamWorkshop.workshopListModel.setCurrentPage(
                                    steamWorkshop.workshopListModel.currentPage - 1)
                        steamWorkshop.searchWorkshop(
                                    SteamEnums.K_EUGCQuery_RankedByTrend)
                    }
                }
                Text {
                    id: txtPage
                    Layout.alignment: Qt.AlignVCenter
                    text: steamWorkshop.workshopListModel.currentPage + "/"
                          + steamWorkshop.workshopListModel.pages
                    font.family: ScreenPlay.settings.font
                    color: Material.primaryTextColor
                }
                Button {
                    id: btnForward
                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Forward")
                    enabled: steamWorkshop.workshopListModel.currentPage
                             <= steamWorkshop.workshopListModel.pages - 1
                    onClicked: {
                        steamWorkshop.workshopListModel.setCurrentPage(
                                    steamWorkshop.workshopListModel.currentPage + 1)
                        steamWorkshop.searchWorkshop(
                                    SteamEnums.K_EUGCQuery_RankedByTrend)
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }

    Sidebar {
        id: sidebar
        topMargin: 60
        steamWorkshop: root.steamWorkshop
        onTagClicked: {
            gridView.headerItem.searchField.text = tag
            sidebar.close()
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:800;width:1366}
}
 ##^##*/

