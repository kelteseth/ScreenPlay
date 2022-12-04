import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlayWorkshop

Rectangle {
    id: root

    property SteamWorkshop steam

    signal uploadPressed

    implicitWidth: 800
    height: 50
    color: Material.theme === Material.Light ? "white" : Material.background

    Item {
        id: wrapper

        height: 50

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            left: parent.left
        }

        Text {
            id: name

            font.pointSize: 14
            color: Material.primaryTextColor
            verticalAlignment: Qt.AlignVCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: {
                return steam.steamAccount.username + qsTr(
                            " Subscribed items: ") + steam.steamAccount.amountSubscribedItems
            }

            anchors {
                top: parent.top
                left: avatar.right
                leftMargin: 10
                bottom: parent.bottom
                right: btnUplaod.left
                rightMargin: 10
            }
        }

        SteamImage {
            id: avatar

            width: 30
            height: 30
            Component.onCompleted: {
                steam.steamAccount.loadAvatar()
            }

            anchors {
                left: parent.left
                leftMargin: 10
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
            source: "qrc:/qml/ScreenPlayWorkshop/assets/images/steam_default_avatar.png"
        }

        Button {
            id: btnUplaod

            text: qsTr("Upload to the Steam Workshop")
            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_plus.svg"
            icon.color: "white"
            icon.width: 16
            icon.height: 16
            onClicked: uploadPressed()

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10
            }
        }
    }

    states: [
        State {
            name: "base"

            PropertyChanges {
                target: bg
                radius: 3
            }
        },
        State {
            name: "scrolling"

            PropertyChanges {
                target: bg
                radius: 0
            }
        }
    ]
}
