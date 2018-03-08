import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: monitors
    state: "inactive"
    focus: true

    property string activeMonitorName: ""

    onStateChanged: {
        bgMouseArea.focus = monitors.state == "active" ? true : false
        if (monitors.state == "active") {


            //monitorListModel.reloadMonitors();
        }
    }

    Rectangle {
        id: background
        color: "#99000000"
        anchors.fill: parent

        MouseArea {
            id: bgMouseArea
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
        }
    }

    Rectangle {
        color: "white"
        radius: 3
        z: 98
        anchors {
            fill: parent
            margins: 50
        }

        Item {
            width: parent.width * .5
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 10
            }
            Text {
                id: txtHeadline
                text: qsTr("Wallpaper Configuration")
                font.pixelSize: 21
                color: "#626262"
                font.family: "Roboto"
                font.weight: Font.Thin
                width: 400
                anchors {
                    top: parent.top
                    topMargin: 10
                    left: parent.left
                    leftMargin: 20
                }
            }
            MonitorSelection {
                id: monitorSelection
                background: "#E4E4E4"
                radius: 3
                height: 200
                z: 99
                width: parent.width * .9
                anchors {
                    top: txtHeadline.bottom
                    topMargin: 20
                    left: parent.left
                    leftMargin: 20
                }
                availableWidth: width - 20
                availableHeight: 150
            }

            Button {
                id: btnRemoveAllWallpaper
                text: qsTr("Remove all wallpaper")
                Material.background: Material.Orange
                Material.foreground: "white"
                onClicked: screenPlay.removeAllWallpaper()
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 30
                }
            }
        }

        GridView {
            id: gridView
            boundsBehavior: Flickable.DragOverBounds
            maximumFlickVelocity: 7000
            flickDeceleration: 5000
            cellWidth: 340
            cacheBuffer: 10000
            width: parent.width * .5
            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
                margins: 10
            }

            delegate: Rectangle {
                color: isHeadline ? "gray" : "steelblue"
                id: delegate
                focus: true
                height: isHeadline ? 50 : 30
                width: 300
                anchors {
                    left: parent.left
                    leftMargin: isHeadline ? 0 : 25
                }

                Text {
                    id: txtDescription
                    text: name
                    width: 100
                    font.pixelSize: isHeadline ? 21 : 14
                    anchors {
                        left: parent.left
                    }
                }

                Rectangle {
                    height: parent.height
                    color: "gray"
                    visible: !isHeadline
                    anchors {
                        left: txtDescription.right
                        leftMargin: 20
                        right: parent.right
                    }

                    TextEdit {
                        anchors.fill: parent
                        anchors.margins: 4
                        focus: true
                        // We need to convert to a string because of large numbers js uses scientific notation x*e^x
                        text: value.toString()
                    }
                }
            }
        }
        MouseArea {
            anchors {
                top:parent.top
                right: parent.right
                margins: 5

            }
            width: 32
            height: width
            onClicked: monitors.state = "inactive"

            Image {
                id: imgClose
                source: "qrc:/assets/icons/font-awsome/close.svg"
                width: 16
                height: 16
                anchors.centerIn: parent
                sourceSize: Qt.size(width,width)
            }
            ColorOverlay {
                id: iconColorOverlay
                anchors.fill: imgClose
                source: imgClose
                color: "gray"
            }
        }
    }



    states: [
        State {
            name: "active"

            PropertyChanges {
                target: monitors
                opacity: 1
                enabled: true
            }

            PropertyChanges {
                target: background
                opacity: 1
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: monitors
                opacity: 0
                enabled: false
            }

            PropertyChanges {
                target: background
                opacity: 0
            }
        }
    ]

    transitions: [
        Transition {

            NumberAnimation {
                target: background
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: monitors
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
