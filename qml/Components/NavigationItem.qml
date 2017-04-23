import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id:navigationItem
    width:150
    height:60
    state: "inactive"

    property string name: "value"
    property string iconSource: "qrc:/assets/icons/icon_installed.svg"

    signal tabClicked(string name)

    function setActive(isActive){
        if(isActive)
        {
           navigationItem.state = "active"
        }else
        {
           navigationItem.state = "inactive"
        }
    }


    MouseArea{
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            navigationItem.tabClicked(navigationItem.name)
        }

        Item {
            id: item2
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: txt
                anchors.left: icon.right
                anchors.leftMargin: 10
                text: name
                font.pointSize: 14
                color: "#626262"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: font_Roboto_Regular.name
                renderType: Text.NativeRendering

                FontLoader{
                    id: font_Roboto_Regular
                    source: "qrc:/assets/fonts/Roboto-Regular.ttf"
                }
            }

            Image {
                id: icon
                source: iconSource
                width:16
                height:16
                sourceSize.height: 16
                sourceSize.width: 16
                fillMode: Image.PreserveAspectFit
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.verticalCenter: parent.verticalCenter
            }

            ColorOverlay {
                id:iconColorOverlay
                anchors.fill: icon
                source: icon
                color: "#FFAB00"

            }



        }

        Rectangle {
            id: navIndicator
            y: 83
            height: 3
            color: "#FFAB00"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: navIndicator
                anchors.bottomMargin: 0
            }

            PropertyChanges {
                target: iconColorOverlay
                color: "#FFAB00"
            }

        },
        State {
            name: "disabled"

            PropertyChanges {
                target: navIndicator
                anchors.bottomMargin: -3
            }

            PropertyChanges {
                target: iconColorOverlay
                color: "#00000000"
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                target: navIndicator
                anchors.bottomMargin: -3
            }

            PropertyChanges {
                target: iconColorOverlay
                color: "#00000000"
            }
        }
    ]

    transitions: [
        Transition {
            to: "active"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 100
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            to: "inactive"

            NumberAnimation {
                properties: "anchors.bottomMargin"
                duration: 100
                easing.type: Easing.InOutQuad
            }
        }
    ]




}
