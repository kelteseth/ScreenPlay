import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

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
            id: txtTitle
            text: workshopItem.name
            opacity: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -50
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
        }
        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onContainsMouseChanged: {
                if(containsMouse){
                    workshopItem.state = "hover"
                } else {
                    workshopItem.state = ""
                }
            }
        }

        Button {
            id: button
            text: qsTr("Subscribe")
            opacity: 0
            Material.background: Material.Orange
            Material.foreground: "white"
            anchors.horizontalCenterOffset: 1
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -50
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                button.enabled = false
                steamWorkshop.subscribeItem(workshopItem.steamID)
            }
        }

    }

    states: [
        State {
            name: "hover"

            PropertyChanges {
                target: button
                opacity: 1
                anchors.bottomMargin: 10
            }

            PropertyChanges {
                target: txtTitle
                opacity: 1
                anchors.bottomMargin: 70
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "hover"
            reversible: true

            PropertyAnimation{
                target:button
                duration: 200
                properties: "opacity, anchors.bottomMargin"
            }
            PropertyAnimation{
                target:txtTitle
                duration: 200
                properties: "opacity, anchors.bottomMargin"
            }
        }
    ]
}
