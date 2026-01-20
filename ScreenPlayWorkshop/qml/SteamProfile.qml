import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
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
                    onClicked: {
                        stackView.pop()
                    }
                }
            }
        }

        GridView {
            id: gridView
            objectName: "profileGridView"

            maximumFlickVelocity: 7000
            flickDeceleration: 5000
            cellWidth: 330
            cellHeight: 220
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
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                }

                Button {
                    id: btnDeleteItem
                    objectName: "btnDeleteItem" + index
                    text: qsTr("Delete")
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg"
                    icon.color: "white"
                    Material.background: Material.Red

                    anchors {
                        top: workshopItem.bottom
                        topMargin: 5
                        horizontalCenter: parent.horizontalCenter
                    }

                    onClicked: {
                        deleteConfirmDialog.publishedFileID = m_publishedFileID
                        deleteConfirmDialog.itemName = m_workshopTitle
                        deleteConfirmDialog.open()
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

    Dialog {
        id: deleteConfirmDialog
        objectName: "deleteConfirmDialog"

        property var publishedFileID
        property string itemName

        title: qsTr("Delete Workshop Item")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent

        Text {
            text: qsTr("Are you sure you want to delete '%1'?\nThis action cannot be undone.").arg(deleteConfirmDialog.itemName)
            color: Material.foreground
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            root.steamWorkshop.deleteItem(publishedFileID)
        }
    }

    Connections {
        target: root.steamWorkshop
        function onWorkshopItemDeleted(success: bool, publishedFileID) {
            if (success) {
                console.log("Workshop item deleted successfully:", publishedFileID)
            } else {
                console.warn("Failed to delete workshop item:", publishedFileID)
            }
        }
    }
}
