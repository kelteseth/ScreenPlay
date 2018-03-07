import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Item {
    id:navigationWallpaperConfiguration
    width: 400
    state: "out"
    property bool isActive: screenPlaySettings.activeWallpaperCounter == 0 ? false : true
    onIsActiveChanged: {
        if(isActive){
            state = "in"
        } else {
            state = "out"
        }
    }

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
        //        opacity:
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
            renderType: Text.NativeRendering
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 1
            }

        }
    }

    Text {
        id: activeMonitorName
        text: qsTr("Wallpaper Configuration")
        anchors {
            right: image.right
            rightMargin: 30
            verticalCenter: parent.verticalCenter
        }
        horizontalAlignment: Text.AlignRight
        color: "#626262"
        font.pointSize: 12
        font.family: "Roboto"
        renderType: Text.NativeRendering
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            utility.setToggleWallpaperConfiguration()
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: navigationWallpaperConfiguration
                opacity: 0
                anchors.topMargin: 100
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: navigationWallpaperConfiguration
                opacity: 1
                anchors.topMargin: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true
            NumberAnimation {
                target: navigationWallpaperConfiguration
                properties: "anchors.topMargin, opacity"
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
