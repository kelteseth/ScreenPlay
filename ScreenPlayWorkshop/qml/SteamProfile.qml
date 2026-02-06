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

    // Only load once on push, not when revealed by popping SteamProfileWorkshopItem
    Component.onCompleted: root.steamWorkshop.requestUserItems()

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

    Flickable {
        id: scrollView

        anchors.fill: parent
        contentWidth: root.width
        contentHeight: gridView.height + header.height + 150

        Item {
            id: header
            height: 120
            anchors {
                top: parent.top
                topMargin: 20
                left: parent.left
                right: parent.right
                leftMargin: 50
                rightMargin: 75
            }

            RowLayout {
                anchors.fill: parent
                anchors.verticalCenter: parent.verticalCenter
                spacing: 16

                Button {
                    id: backButton
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_arrow_left.svg"
                    icon.color: "white"
                    flat: true
                    onClicked: root.stackView.pop()
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Back to Workshop")
                }

                SteamImage {
                    id: avatar

                    Layout.preferredWidth: 64
                    Layout.preferredHeight: 64
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
                    spacing: 2

                    Label {
                        text: root.steamWorkshop.steamAccount.username
                        font.pointSize: 14
                        font.bold: true
                        color: "white"
                    }

                    Label {
                        text: qsTr("Workshop Creator")
                        font.pointSize: 9
                        color: Qt.rgba(1, 1, 1, 0.7)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                ProfileStatCard {
                    title: qsTr("Published")
                    value: root.steamWorkshop.userPublishedItemCount.toLocaleString()
                    iconSource: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_file_upload.svg"
                }

                ProfileStatCard {
                    title: qsTr("Subscribers")
                    value: root.steamWorkshop.userTotalSubscriptions.toLocaleString()
                    iconSource: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
                }
            }
        }

        SPCore.MaterialGridView {
            id: gridView
            objectName: "profileGridView"

            cellWidth: 320
            cellHeight: 180
            height: contentHeight
            interactive: false
            model: root.steamWorkshop.workshopProfileListModel
            boundsBehavior: Flickable.StopAtBounds

            anchors {
                top: header.bottom
                topMargin: 20
                left: parent.left
                right: parent.right
                leftMargin: 50
                rightMargin: 55
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
                    text: qsTr("Previous")
                    enabled: root.steamWorkshop.workshopProfileListModel.currentPage > 1
                    onClicked: {
                        root.steamWorkshop.workshopProfileListModel.setCurrentPage(root.steamWorkshop.workshopProfileListModel.currentPage - 1)
                    }
                }

                Label {
                    id: txtPage

                    Layout.alignment: Qt.AlignVCenter
                    text: root.steamWorkshop.workshopProfileListModel.currentPage + "/" + root.steamWorkshop.workshopProfileListModel.pages
                    color: Material.primaryTextColor
                }

                Button {
                    id: btnForward

                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("Next")
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

    component ProfileStatCard: Rectangle {
        id: statCard

        required property string title
        required property string value
        required property string iconSource

        color: Material.dialogColor
        radius: 8
        implicitWidth: 90
        implicitHeight: 70

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 4

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 4

                Image {
                    source: statCard.iconSource
                    sourceSize: Qt.size(12, 12)
                    opacity: 0.7
                }

                Label {
                    text: statCard.title
                    font.pointSize: 9
                    color: Material.secondaryTextColor
                }
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: statCard.value
                font.pointSize: 14
                font.bold: true
                color: Material.foreground
            }
        }
    }
}
