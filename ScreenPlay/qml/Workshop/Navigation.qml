import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material.impl 2.12

import ScreenPlay.Workshop 1.0
import SteamQMLImageProvider 1.0
import ScreenPlay 1.0

Rectangle {
    id: root
    implicitWidth: 800
    height: 50

    property SteamWorkshop steamWorkshop

    signal uploadPressed
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
            text: {
                return steamWorkshop.steamAccount.username + qsTr(
                            " Subscribed items: ")
                        + steamWorkshop.steamAccount.amountSubscribedItems
            }

            font.pointSize: 14
            color: Material.primaryTextColor
            font.family: ScreenPlay.settings.font
            font.weight: Font.Thin
            verticalAlignment: Qt.AlignVCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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
            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
            Component.onCompleted: {
                steamWorkshop.steamAccount.loadAvatar()
            }
            Connections {
                target: steamWorkshop.steamAccount
                function onAvatarChanged(_avatar) {
                    avatar.setImage(_avatar)
                }
            }
        }

        Button {
            id: btnUplaod
            text: qsTr("Upload to the Steam Workshop")
            icon.source: "qrc:/assets/icons/icon_plus.svg"
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
