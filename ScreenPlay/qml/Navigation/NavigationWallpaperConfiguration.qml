import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0
import ScreenPlay 1.0
import "../Common"

Item {
    id: root

    width: 390
    property bool active: ScreenPlay.screenPlayManager.activeWallpaperCounter > 0

    anchors {
        top: parent.top
        right: parent.right
        rightMargin: 20
        bottom: parent.bottom
    }

    Connections {
        function onActiveWallpaperCounterChanged() {
            rippleEffect.trigger()
        }

        target: ScreenPlay.screenPlayManager
    }

    Pane {
        anchors.centerIn: parent
        contentWidth: activeMonitorName.contentWidth + image.width + 80
        contentHeight: parent.height - 10
        background: Item {
            Rectangle {
                id: bg
                clip: true
                anchors {
                    fill: parent
                    margins: 16
                }
                color: "transparent"
                border.color: root.active ? Material.accentColor : Material.iconDisabledColor
                border.width: 1
                radius: 3
            }

            RippleEffect {
                id: rippleEffect

                target: bg
            }

        }

        Behavior on contentWidth {
            PropertyAnimation {
                duration: 250
                easing.type: Easing.InOutQuart
            }
        }

        Text {
            id: activeMonitorName

            horizontalAlignment: Text.AlignRight
            color: root.active ? Material.primaryTextColor : Material.iconDisabledColor
            font.pointSize: 12
            font.family: ScreenPlay.settings.font
            text: root.active ? qsTr("Configurate active Wallpaper or Widgets") : qsTr(
                                    "No active Wallpaper or Widgets")

            anchors {
                right: image.right
                rightMargin: 50
                verticalCenter: parent.verticalCenter
            }
        }

        Image {
            id: image

            width: 28
            height: 24
            source: "qrc:/assets/icons/icon_monitor.svg"
            layer {
                effect: ColorOverlay {
                    color:root.active ?  Material.color(Material.Orange) : Material.iconDisabledColor
                }
            }

            anchors {
                rightMargin: 30
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            Text {
                id: txtAmountActiveWallpapers

                text: ScreenPlay.screenPlayManager.activeWallpaperCounter
                      + ScreenPlay.screenPlayManager.activeWidgetsCounter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color:root.active ?  Material.color(Material.Orange) : Material.iconDisabledColor
                font.pointSize: 10
                font.bold: true
                font.family: ScreenPlay.settings.font
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 7
                }
            }
        }

        MouseArea {
            id: ma

            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                ScreenPlay.util.setToggleWallpaperConfiguration()
            }
        }
    }
}
