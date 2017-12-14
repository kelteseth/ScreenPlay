import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {
    id: workshopItem
    width: 320
    height: 180

    property url imgUrl
    property string name
    property int steamID

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    RectangularGlow {
        id: effect
        anchors.fill: itemWrapper
        cached: true
        glowRadius: 2
        spread: 0.5
        color: "black"
        opacity: .2
        cornerRadius: itemWrapper.radius + glowRadius
    }

    Rectangle {
        id: itemWrapper
        color: "white"
        radius: 2
        anchors {
            fill: parent
            margins: 5
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {

            }
        }

        Image {
            id: img
            anchors.fill: parent
            asynchronous: true
            clip: true
            cache: true
            source: workshopItem.imgUrl
            smooth: false
        }
        Text {
            id: namea
            anchors.centerIn: parent
            text: workshopItem.name
            color: "white"
        }

        Button {
            id: button
            text: qsTr("Subscribe")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: steamWorkshop.subscribeItem(workshopItem.steamID)
        }
    }
}
