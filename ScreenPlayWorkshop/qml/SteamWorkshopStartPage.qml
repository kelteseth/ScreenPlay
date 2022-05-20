import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import ScreenPlayWorkshop
import WorkshopEnums
import "upload/"

Item {
    id: root

    property SteamWorkshop steamWorkshop
    property ScreenPlayWorkshop screenPlayWorkshop
    property StackView stackView
    property Background background

    Component.onCompleted: {
        root.state = "searching"
        searchConnection.target = root.steamWorkshop
        root.steamWorkshop.searchWorkshop(SteamEnums.K_EUGCQuery_RankedByTrend)
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
        ignoreUnknownSignals: true // This is needed for some reason...
        function onWorkshopBannerCompleted() {
            bannerTxt.text = root.steamWorkshop.workshopListModel.getBannerText(
                        )
            background.backgroundImage = root.steamWorkshop.workshopListModel.getBannerUrl()
            banner.bannerPublishedFileID = root.steamWorkshop.workshopListModel.getBannerID()
            bannerTxtUnderline.numberSubscriber
                    = root.steamWorkshop.workshopListModel.getBannerAmountSubscriber()
        }

        function onWorkshopSearchCompleted(itemCount) {
            root.state = ""
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

                    Rectangle {
                        height: 36
                        width: 160
                        color: Material.backgroundColor
                        radius: 4
                        Text {
                            id: bannerTxtUnderline

                            property int numberSubscriber: 0

                            text: numberSubscriber + " SUBSCRIBED TO:"
                            font.pointSize: 12
                            color: "white"
                            font.weight: Font.Thin
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                        }
                    }

                    Text {
                        id: bannerTxt

                        text: qsTr("Loading")
                        font.pointSize: 42
                        color: "white"
                        font.weight: Font.Thin
                        width: 400

                        layer.enabled: true
                        layer.effect: DropShadow {
                            verticalOffset: 2
                            color: "#80000000"
                            radius: 3
                        }
                    }

                    RowLayout {
                        spacing: 10

                        Button {
                            text: qsTr("Download now!")
                            Material.accent: Material.color(Material.Orange)
                            highlighted: true
                            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
                            onClicked: {
                                text = qsTr("Downloading...")
                                root.steamWorkshop.subscribeItem(
                                            root.steamWorkshop.workshopListModel.getBannerID(
                                                ))
                            }
                        }

                        Button {
                            text: qsTr("Details")
                            Material.accent: Material.color(Material.Orange)
                            highlighted: true
                            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_info.svg"
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
                            font.weight: Font.Thin
                            layer.enabled: true
                            layer.effect: DropShadow {
                                verticalOffset: 2
                                color: "#80000000"
                                radius:3
                            }
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
            model: root.steamWorkshop.workshopListModel
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
                            steamWorkshop.steamAccount.loadAvatar()
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

                            target: steamWorkshop.steamAccount
                        }
                    }

                    Image {
                        id: avatarPlaceholder
                        anchors.fill: avatar
                        source: "qrc:/qml/ScreenPlayWorkshop/assets/images/steam_default_avatar.png"
                    }

                    Button {
                        id: btnSteamProfile

                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: avatar.right
                            leftMargin: 20
                        }

                        text: qsTr("Profile")
                        icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_account_circle.svg"
                        onClicked: {
                            stackView.push(
                                        "qrc:/qml/ScreenPlayWorkshop/qml/SteamProfile.qml",
                                        {
                                            "screenPlayWorkshop": root.screenPlayWorkshop,
                                            "steamWorkshop": root.steamWorkshop
                                        })
                        }
                    }

                    Button {
                        id: btnSteamUpload

                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: btnSteamProfile.right
                            leftMargin: 20
                        }

                        text: qsTr("Upload")
                        icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_file_upload.svg"
                        onClicked: {
                            stackView.push(
                                        "qrc:/qml/ScreenPlayWorkshop/qml/upload/UploadProject.qml",
                                        {
                                            "screenPlayWorkshop": root.screenPlayWorkshop,
                                            "steamWorkshop": root.steamWorkshop
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
                        ToolButton {
                            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_search.svg"
                            onClicked: {
                                root.state = "searching"
                                root.steamWorkshop.searchWorkshopByText(
                                            tiSearch.text)
                            }
                            icon.width: 20
                            icon.height: 20
                            anchors {
                                left: parent.left
                                leftMargin: -3
                                bottom: parent.bottom
                                bottomMargin: 3
                            }
                        }

                        TextField {
                            id: tiSearch
                            placeholderTextColor: Material.secondaryTextColor
                            placeholderText: qsTr("Search for Wallpaper and Widgets...")
                            onEditingFinished: {
                                root.state = "searching"
                                if (tiSearch.text === "")
                                    return root.steamWorkshop.searchWorkshop(
                                                SteamEnums.K_EUGCQuery_RankedByTrend)

                                root.steamWorkshop.searchWorkshopByText(
                                            tiSearch.text)
                            }
                            leftInset: -20
                            leftPadding: 20
                            anchors {
                                top: parent.top
                                right: parent.right
                                bottom: parent.bottom
                                left: parent.left
                                leftMargin: 20
                            }
                        }
                        ToolButton {
                            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg"
                            onClicked: {
                                root.state = "searching"
                                tiSearch.text = ""
                                root.steamWorkshop.searchWorkshop(SteamEnums.K_EUGCQuery_RankedByTrend)
                            }
                            enabled: tiSearch.text !== ""
                            icon.width: 20
                            icon.height: 20
                            anchors {
                                right: parent.right
                                rightMargin: -3
                                bottom: parent.bottom
                                bottomMargin: 3
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
                            onClicked: Qt.openUrlExternally(
                                           "steam://url/SteamWorkshopPage/672870")
                            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_steam.svg"
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
                            root.state = "searching"
                            root.steamWorkshop.searchWorkshop(
                                        cbQuerySort.currentValue)
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
                steamWorkshop: root.steamWorkshop
                onClicked: {
                    sidebar.setWorkshopItem(m_publishedFileID,
                                            m_workshopPreview,
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
                    enabled: root.steamWorkshop.workshopListModel.currentPage > 1
                    onClicked: {
                        root.state = "searching"
                        root.steamWorkshop.workshopListModel.setCurrentPage(
                                    root.steamWorkshop.workshopListModel.currentPage - 1)
                        root.steamWorkshop.searchWorkshop(
                                    SteamEnums.K_EUGCQuery_RankedByTrend)
                    }
                }

                Text {
                    id: txtPage

                    Layout.alignment: Qt.AlignVCenter
                    text: root.steamWorkshop.workshopListModel.currentPage + "/"
                          + root.steamWorkshop.workshopListModel.pages
                    color: Material.primaryTextColor
                }

                Button {
                    id: btnForward

                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Forward")
                    enabled: root.steamWorkshop.workshopListModel.currentPage
                             <= root.steamWorkshop.workshopListModel.pages - 1
                    onClicked: {
                        root.state = "searching"
                        root.steamWorkshop.workshopListModel.setCurrentPage(
                                    root.steamWorkshop.workshopListModel.currentPage + 1)
                        root.steamWorkshop.searchWorkshop(
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
        steamWorkshop: root.steamWorkshop
        onTagClicked: {
            gridView.headerItem.searchField.text = tag
            sidebar.close()
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