import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayWorkshop
import ScreenPlayCore as SPCore
import "upload/"

Item {
    id: root

    property StackView stackView
    property ScreenPlayWorkshop screenPlayWorkshop
    property SteamWorkshop steamWorkshop
    property Background background

    Component.onCompleted: {
        root.state = "searching"
        root.steamWorkshop.search.searchWorkshopByText("")
    }

    onVisibleChanged: {
        if (!visible)
            sidebar.close()
    }

    MouseArea {
        id: rootMouseArea
        enabled: false
        z: enabled ? 10 : 0
        cursorShape: enabled ? Qt.WaitCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
        anchors.fill: parent
        preventStealing: true
    }

    Connections {
        id: searchConnection
        target: root.steamWorkshop.search

        property string bannerCreatorSteamID: ""

        function onWorkshopBannerCompleted(): void {
            const info = root.steamWorkshop.search.workshopListModel.getBannerInfo()
            root.background.backgroundImage = info.imageUrl
            gridView.headerItem.bannerInfo = info
            gridView.headerItem.creatorName = ""
            gridView.headerItem.creatorSteamID = ""
            searchConnection.bannerCreatorSteamID = info.creatorSteamID
        }

        function onWorkshopSearchCompleted(itemCount: int): void {
            root.state = ""
        }
    }

    Connections {
        target: root.steamWorkshop

        function onCreatorNameReady(name: string, steamID64: string): void {
            if (steamID64 === searchConnection.bannerCreatorSteamID) {
                gridView.headerItem.creatorName = name
                gridView.headerItem.creatorSteamID = steamID64
            }
        }
    }

    PopupSteamWorkshopAgreement {
        id: popupSteamWorkshopAgreement
    }

    Connections {
        function onUserNeedsToAcceptWorkshopLegalAgreement() {
            popupSteamWorkshopAgreement.open()
        }

        target: root.steamWorkshop.uploadListModel
    }

    SPCore.MaterialGridView {
        id: gridView

        property bool isScrolling: gridView.verticalVelocity !== 0
        readonly property int itemsPerRow: Math.floor(gridView.width / gridView.cellWidth)

        cellWidth: 330
        cellHeight: 190
        model: root.steamWorkshop.search.workshopListModel
        boundsBehavior: Flickable.StopAtBounds

        anchors {
            fill: parent
            leftMargin: 45
        }

        onContentYChanged: {
            // Calculate parallax scrolling
            if (root.background) {
                if (contentY >= 0)
                    root.background.imageOffsetTop = (contentY * -0.8)
                else
                    root.background.imageOffsetTop = 0
            }

            // Endless scrolling: load more when near the bottom
            gridView.checkLoadMore()
        }

        function checkLoadMore(): void {
            if (!gridView.atYEnd)
                return
            if (!root.steamWorkshop)
                return
            const model = root.steamWorkshop.search.workshopListModel
            if (!model)
                return
            if (model.hasMore && !model.isLoading) {
                root.steamWorkshop.search.loadNextPage()
            }
        }

        header: Item {
            id: header
            property alias searchField: tiSearch
            property bannerInfo bannerInfo
            property string creatorName: ""
            property string creatorSteamID: ""

            height: 450 + 100 + 90 // banner + topMargin + search bar
            width: gridView.width - gridView.anchors.leftMargin

            Item {
                id: banner

                height: 450

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

                    spacing: 10

                    HeaderLabel {
                        id: bannerTxtUnderline
                        text: header.bannerInfo.subscriptionCount + " SUBSCRIBED TO:"
                        font.pointSize: 12
                    }

                    HeaderLabel {
                        id: bannerTxt
                        text: header.bannerInfo.title
                        font.pointSize: 42
                    }

                    HeaderLabel {
                        id: bannerCreatorLabel

                        text: header.creatorName ? qsTr("By %1").arg(header.creatorName) : ""
                        visible: header.creatorName !== ""
                        font.pointSize: 14
                        font.underline: true

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                gridView.headerItem.searchField.text = header.creatorName
                                root.steamWorkshop.search.searchWorkshopByUser(header.creatorSteamID)
                            }
                        }
                    }

                    RowLayout {
                        spacing: 10

                        Button {
                            id: btnBannerDownload
                            text: qsTr("Download now!")
                            Material.accent: Material.color(Material.Orange)
                            highlighted: true
                            icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
                            onClicked: {
                                text = qsTr("Downloading...")
                                root.steamWorkshop.itemOps.subscribeItem(root.steamWorkshop.search.workshopListModel.getBannerInfo().publishedFileID)
                            }

                            Connections {
                                target: root.steamWorkshop
                                function onWorkshopItemInstalled() {
                                    btnBannerDownload.text = qsTr("Download now!")
                                }
                            }
                        }

                        RowLayout {
                            spacing: 20
                            Button {
                                text: qsTr("Details")
                                Material.accent: Material.color(Material.Orange)
                                highlighted: true
                                //icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_info.svg"
                                visible: false
                                onClicked: {
                                    sidebar.setWorkshopItem(publishedFileID, imgUrl, additionalPreviewUrl, subscriptionCount)
                                }
                            }
                            ToolButton {
                                onClicked: Qt.openUrlExternally("steam://url/CommunityFilePage/" + header.bannerInfo.publishedFileID)
                                icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
                            }
                        }
                    }
                }
            }

            SPCore.ImageBlurContainer {
                id: searchBar
                backgroundSource: root.background
                flickable: gridView
                stackView: root.stackView
                radius: 8
                width: parent.width - 10
                height: 70
                clip: true

                anchors {
                    top: banner.bottom
                    topMargin: 100
                    horizontalCenter: parent.horizontalCenter
                }

                SteamImage {
                    id: avatar

                    width: 55
                    height: 55
                    Component.onCompleted: {
                        steamWorkshop.steamAccount.loadAvatar()
                    }

                    anchors {
                        left: parent.left
                        leftMargin: 8
                        verticalCenter: parent.verticalCenter
                    }

                    Connections {
                        function onAvatarChanged(_avatar: var): void {
                            avatar.setImage(_avatar)
                            avatarPlaceholder.opacity = 0
                        }

                        target: steamWorkshop.steamAccount
                    }
                }

                Image {
                    id: avatarPlaceholder
                    anchors.fill: avatar
                    source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/images/steam_default_avatar.png"
                }

                Button {
                    id: btnSteamProfile
                    objectName: "btnSteamProfile"

                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: avatar.right
                        leftMargin: 20
                    }

                    text: qsTr("Profile")
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_account_circle.svg"
                    onClicked: {
                        stackView.push("qrc:/qt/qml/ScreenPlayWorkshop/qml/SteamProfile.qml", {
                            "screenPlayWorkshop": root.screenPlayWorkshop,
                            "steamWorkshop": root.steamWorkshop,
                            "stackView": root.stackView,
                            "background": root.background
                        })
                    }
                }

                Button {
                    id: btnSteamUpload
                    objectName: "btnSteamUpload"

                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: btnSteamProfile.right
                        leftMargin: 20
                    }

                    text: qsTr("Upload")
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_file_upload.svg"
                    onClicked: {
                        stackView.push("qrc:/qt/qml/ScreenPlayWorkshop/qml/upload/SteamUploadView.qml", {
                            "screenPlayWorkshop": root.screenPlayWorkshop,
                            "steamWorkshop": root.steamWorkshop,
                            "stackView": root.stackView
                        })
                    }
                }

                Item {
                    id: searchWrapper

                    height: 50

                    anchors {
                        left: btnSteamUpload.right
                        right: wrapperRight.left
                        rightMargin: 20
                        leftMargin: 20
                        verticalCenter: parent.verticalCenter
                    }

                    TextField {
                        id: tiSearch
                        placeholderTextColor: Material.secondaryTextColor
                        placeholderText: qsTr("Search wallpapers, or filter by \"tag\" using quotes")
                        Keys.onReturnPressed: event => {
                            event.accepted = true
                            tiSearch.searchWorkshop()
                        }

                        onEditingFinished: tiSearch.searchWorkshop()
                        function searchWorkshop(): void {
                            if (root.state === "searching") {
                                print("SEARCHING")
                                return
                            }
                            root.state = "searching"
                            if (tiSearch.text === "") {
                                Qt.callLater(function () {
                                    root.steamWorkshop.search.searchWorkshop(SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByTrend)
                                })
                                return
                            }
                            Qt.callLater(function () {
                                root.steamWorkshop.search.searchWorkshopByText(tiSearch.text)
                            })
                        }

                        anchors {
                            top: parent.top
                            right: parent.right
                            bottom: parent.bottom
                            left: parent.left
                            leftMargin: 20
                        }
                    }

                    ToolButton {
                        property bool hasContent: tiSearch.text.length > 0
                        icon.source: hasContent ? "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg" : "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_search.svg"
                        onClicked: {
                            if (hasContent) {
                                root.state = "searching"
                                tiSearch.clear()
                                Qt.callLater(function () {
                                    root.steamWorkshop.search.searchWorkshop(SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByTrend)
                                })
                                return
                            }
                            root.state = "searching"
                            Qt.callLater(function () {
                                root.steamWorkshop.search.searchWorkshopByText(tiSearch.text)
                            })
                        }
                        icon.width: 20
                        icon.height: 20
                        anchors {
                            right: parent.right
                            rightMargin: 0
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }

                RowLayout {
                    id: wrapperRight
                    spacing: 20

                    anchors {
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
                        onClicked: Qt.openUrlExternally("steam://url/SteamWorkshopPage/672870")
                        icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_steam.svg"
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
                    model: [
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByVote,
                            "text": qsTr("Ranked By Vote")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByPublicationDate,
                            "text": qsTr("Publication Date")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByTrend,
                            "text": qsTr("Ranked By Trend")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_FavoritedByFriendsRankedByPublicationDate,
                            "text": qsTr("Favorited By Friends")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_CreatedByFriendsRankedByPublicationDate,
                            "text": qsTr("Created By Friends")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate,
                            "text": qsTr("Created By Followed Users")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_NotYetRated,
                            "text": qsTr("Not Yet Rated")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByTotalVotesAsc,
                            "text": qsTr("Total VotesAsc")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByVotesUp,
                            "text": qsTr("Votes Up")
                        },
                        {
                            "value": SPCore.Steam.EUGCQuery.K_EUGCQuery_RankedByTotalUniqueSubscriptions,
                            "text": qsTr("Total Unique Subscriptions")
                        }
                    ]
                    onActivated: {
                        root.state = "searching"
                        root.steamWorkshop.search.searchWorkshop(cbQuerySort.currentValue)
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
            additionalPreviewWebpUrl: m_additionalPreviewWebpUrl
            subscriptionCount: m_subscriptionCount
            itemIndex: index
            itemsPerRow: gridView.itemsPerRow
            isScrolling: gridView.isScrolling
            steamWorkshop: root.steamWorkshop
            onClicked: {
                sidebar.setWorkshopItem(m_publishedFileID, m_workshopPreview, additionalPreviewUrl, subscriptionCount)
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: workshopScrollBar

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
                    running: root.steamWorkshop.search.workshopListModel.isLoading
                    visible: running
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Loading more...")
                    color: Material.secondaryTextColor
                    visible: root.steamWorkshop.search.workshopListModel.isLoading
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("No more items")
                    color: Material.secondaryTextColor
                    visible: !root.steamWorkshop.search.workshopListModel.hasMore && !root.steamWorkshop.search.workshopListModel.isLoading && root.steamWorkshop.search.workshopListModel.currentPage > 1
                }
            }
        }
    }

    component HeaderLabel: Label {
        id: textObj
        color: "white"
        wrapMode: Text.WrapAnywhere
        clip: true

        width: Math.min(100, textWidth)
        readonly property alias textWidth: textMetrics.boundingRect.width

        TextMetrics {
            id: textMetrics
            font: textObj.font
            text: textObj.text
            elide: textObj.elide
        }

        background: SPCore.ImageBlurContainer {
            backgroundSource: root.background
            flickable: gridView
            stackView: root.stackView
        }
        padding: 10
    }

    Sidebar {
        id: sidebar

        topMargin: 60
        steamWorkshop: root.steamWorkshop
        onTagClicked: tag => {
            const quoted = '"' + tag + '"'
            gridView.headerItem.searchField.text = quoted
            root.steamWorkshop.search.searchWorkshopByText(quoted)
            sidebar.close()
        }
        onCreatorSearchRequested: (creatorName, creatorSteamID) => {
            gridView.headerItem.searchField.text = creatorName
            root.steamWorkshop.search.searchWorkshopByUser(creatorSteamID)
        }
    }

    states: [
        State {
            name: "searching"
            PropertyChanges {
                target: rootMouseArea
                enabled: true
            }
        }
    ]
}
