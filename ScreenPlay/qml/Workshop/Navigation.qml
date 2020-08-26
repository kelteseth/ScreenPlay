import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material.impl 2.12

import ScreenPlay.Workshop 1.0
import ScreenPlay.Workshop.SteamEnums 1.0
import SteamQMLImageProvider 1.0
import ScreenPlay 1.0

Item {
    id: root
    width: 800
    height: 60

    signal uploadPressed

    Rectangle {
        id: bg
        color: Material.theme === Material.Light ? "white" : Qt.darker(Material.background)
        opacity: .9
        radius: 3
        anchors.fill: wrapper
        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 4
        }
    }

    Item {
        id: wrapper

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            bottom: parent.bottom
            bottomMargin: 5
        }

        Text {
            id: name
            text: {
                return Workshop.steamWorkshop.steamAccount.username + qsTr(
                            " Subscribed items: ")
                        + Workshop.steamWorkshop.steamAccount.amountSubscribedItems
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
            width: 40
            height: 40
            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
            Component.onCompleted: Workshop.steamWorkshop.steamAccount.loadAvatar()
            Connections {
                target: Workshop.steamWorkshop.steamAccount
                function onAvatarChanged(_avatar) {
                    avatar.setImage(_avatar)
                }
            }
        }

        Button {
            id: btnUplaod
            text: qsTr("Upload to the Steam Workshop")
            Material.background: Material.accent
            Material.foreground: "white"
            icon.source: "qrc:/assets/icons/icon_plus.svg"
            icon.color: "white"
            icon.width: 16
            icon.height: 16
            highlighted: true

            onClicked: uploadPressed()
            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 10
                bottom: parent.bottom
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
