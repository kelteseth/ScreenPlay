import QtQuick 2.9
import QtGraphicalEffects 1.0

Item {
    id: workshopAltertBannerWrapper
    height: 50
    state: "out"

    signal openCreate

    anchors {
        top: parent.top
        right: parent.right
        left: parent.left
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    Rectangle {
        id: workshopAltertBanner
        height: 50
        color: "#3498db"
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        Image {
            id: icoFrown
            source: "qrc:/assets/icons/font-awsome/frown-o.svg"
            sourceSize: Qt.size(18, 18)
            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }
        Text {
            id: name
            text: qsTr("Oh No! Looks like there a only a few wallpapers and widgets! Help us by creating your own wallpapers and Widgets :)")
            color: "white"
            font.pointSize: 11
            renderType: Text.NativeRendering
            font.family: font_Roboto_Regular.name
            anchors {
                left: icoFrown.right
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
        }
        MouseArea {
            anchors {
                top: parent.top
                right: closeWrapper.left
                bottom: parent.bottom
                left: parent.left
            }
            onClicked: {
                screenPlaySettings.setHasWorkshopBannerSeen(true)
                openCreate()
            }
        }

        Item {
            id: closeWrapper
            height: 50
            width: 50
            anchors {
                top: parent.top
                right: parent.right
            }
            Image {
                id: icoClose
                source: "qrc:/assets/icons/font-awsome/close.svg"
                sourceSize: Qt.size(15, 15)
                anchors.centerIn: parent
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    screenPlaySettings.setHasWorkshopBannerSeen(true)
                    workshopAltertBannerWrapper.state = "out"
                }
            }
        }
    }
    states: [
        State {
            name: "out"
            PropertyChanges {
                target: workshopAltertBanner
                anchors.topMargin: -50
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: workshopAltertBanner
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
                target: footer
                property: "anchors.topMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
