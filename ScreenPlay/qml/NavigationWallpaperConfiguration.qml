import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Item {
    id: navigationWallpaperConfiguration
    width: 400

    anchors {
        top: parent.top
        right: parent.right
        rightMargin: 30
        bottom: parent.bottom
    }

    Image {
        id: image
        width: 24
        height: 24
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        source: "qrc:/assets/icons/icon_monitor.svg"

        Text {
            id: txtAmountActiveWallpapers
            text: screenPlaySettings.activeWallpaperCounter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "orange"
            font.pointSize: 10
            font.bold: true
            font.family: "Roboto"

            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 1
            }
        }
    }

    Text {
        id: activeMonitorName
        text: {
            if (screenPlaySettings.activeWallpaperCounter > 0) {
                return qsTr("Configurate active Wallpaper or Widgets")
            } else {
                return qsTr("No active Wallpaper or Widgets")
            }
        }

        anchors {
            right: image.right
            rightMargin: 30
            verticalCenter: parent.verticalCenter
        }
        horizontalAlignment: Text.AlignRight
        color: "#626262"
        font.pointSize: 12
        font.family: "Roboto"
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            utility.setToggleWallpaperConfiguration()
        }
    }

    states: []

    transitions: []
}
