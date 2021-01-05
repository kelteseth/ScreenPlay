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
    id: workshop
    state: "base"
    onVisibleChanged: {
        if (!visible)
            sidebar.close()
    }

    Component.onCompleted: {
        if (Workshop.steamWorkshop.online) {
            Workshop.steamWorkshop.workshopListModel.searchWorkshop(
                        SteamEnums.K_EUGCQuery_RankedByTrend)
        } else {
            popupOffline.open()
        }
    }

    Connections {
        target: Workshop.steamWorkshop.workshopListModel
        function onWorkshopSearched() {
            bannerTxt.text = Workshop.steamWorkshop.workshopListModel.getBannerText()
            background.backgroundImage = Workshop.steamWorkshop.workshopListModel.getBannerUrl()
            banner.bannerWorkshopID = Workshop.steamWorkshop.workshopListModel.getBannerID()
            bannerTxtUnderline.numberSubscriber
                    = Workshop.steamWorkshop.workshopListModel.getBannerAmountSubscriber()
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
    }

    PopupSteamWorkshopAgreement {
        id: popupSteamWorkshopAgreement
    }

    Connections {
        target: Workshop.steamWorkshop.uploadListModel
        function onUserNeedsToAcceptWorkshopLegalAgreement() {
            popupSteamWorkshopAgreement.open()
        }
    }

    Flickable {
        id: scrollView
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: gridView.height + header.height + 300

        onContentYChanged: {
            // Calculate parallax scrolling
            if (contentY >= 0) {
                background.imageOffsetTop = (contentY * -.4)
            } else {
                background.imageOffsetTop = 0
            }
            if (contentY >= (header.height)) {
                workshop.state = "scrolling"
            } else {
                workshop.state = "base"
            }
        }

        transitions: Transition {
            PropertyAnimation {
                properties: "y"
                easing.type: Easing.InOutQuad
                duration: 300
            }
        }

        // This wrapper is needed for the parent change
        // of the nav. Otherwhise it wont work. Dunno why
        Item {
            id: wrapper
            width: parent.width
            height: nav.height + header.height + gridView.height

            Navigation {
                id: nav
                anchors.horizontalCenter: parent.horizontalCenter
                onUploadPressed: popupUploadProject.open()
            }

            Item {
                id: header
                height: 440
                anchors {
                    right: parent.right
                    left: parent.left
                }

                Item {
                    id: banner
                    height: 350
                    z: 5
                    property int bannerWorkshopID
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
                        height: {

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
                                    Workshop.steamWorkshop.subscribeItem(
                                                Workshop.steamWorkshop.workshopListModel.getBannerID(
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
                                    sidebar.setWorkshopItem(
                                                workshopID, imgUrl,
                                                additionalPreviewUrl,
                                                subscriptionCount)
                                }
                            }
                        }

                        MouseArea {
                            onClicked: Qt.openUrlExternally(
                                           "steam://url/CommunityFilePage/"
                                           + banner.bannerWorkshopID)
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
                model: Workshop.steamWorkshop.workshopListModel
                anchors {
                    top: header.bottom
                    topMargin: 100
                    left: parent.left
                    right: parent.right
                    leftMargin: 50
                }

                header: Item {
                    height: 70
                    width: parent.width

                    Item {
                        id: searchWrapper

                        width: 400
                        height: 50
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: Material.theme === Material.Light ? "white" : Qt.darker(
                                                                           Material.background)
                            opacity: .95
                            radius: 3
                        }

                        Common.TextField {
                            id: tiSearch
                            anchors {
                                top: parent.top
                                right: parent.right
                                rightMargin: 10
                                bottom: parent.bottom
                                left: parent.left
                                leftMargin: 10
                            }
                            placeholderText: qsTr("Search for Wallpaper and Widgets...")
                            onTextChanged: timerSearch.restart()
                            Timer {
                                id: timerSearch
                                interval: 300
                                onTriggered: Workshop.steamWorkshop.workshopListModel.searchWorkshopByText(
                                                 tiSearch.text)
                            }
                        }
                    }

                    Row {
                        spacing: 20
                        anchors {
                            left: searchWrapper.right
                            leftMargin: 20
                            right: cbQuerySort.left
                            rightMargin: 20
                            verticalCenter: parent.verticalCenter
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
                        }
                    }

                    ComboBox {
                        id: cbQuerySort
                        width: 250
                        height: searchWrapper.height
                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: 50
                        }

                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 2
                        Layout.preferredHeight: searchWrapper.height
                        font.family: ScreenPlay.settings.font
                        onActivated: {
                            Workshop.steamWorkshop.workshopListModel.searchWorkshop(
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

                boundsBehavior: Flickable.StopAtBounds

                delegate: WorkshopItem {
                    imgUrl: m_workshopPreview
                    name: m_workshopTitle
                    workshopID: m_workshopID
                    additionalPreviewUrl: m_additionalPreviewUrl
                    subscriptionCount: m_subscriptionCount
                    itemIndex: index
                    onClicked: {
                        sidebar.setWorkshopItem(workshopID, imgUrl,
                                                additionalPreviewUrl,
                                                subscriptionCount)
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    id: workshopScrollBar
                    snapMode: ScrollBar.SnapOnRelease
                }
            }
        }
    }

    Sidebar {
        id: sidebar
        topMargin: 60
    }

    states: [
        State {
            name: "base"

            ParentChange {
                target: nav
                parent: wrapper
            }

            PropertyChanges {
                target: nav
                anchors.top: wrapper.top
                anchors.topMargin: header.height
                width: 800
                state: "base"
            }
        },
        State {
            name: "scrolling"
            ParentChange {
                target: nav
                parent: workshop
            }
            PropertyChanges {
                target: nav
                anchors.topMargin: 0
                anchors.top: workshop.top
                width: wrapper.width
                state: "scrolling"
            }
        }
    ]

    transitions: [
        Transition {
            from: "base"
            to: "scrolling"
            PropertyAnimation {
                target: nav
                properties: "width"
                duration: 100
            }
        },
        Transition {
            from: "scrolling"
            to: "base"

            PropertyAnimation {
                target: nav
                properties: "width,x,y"
                duration: 300
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:800;width:1366}
}
 ##^##*/

