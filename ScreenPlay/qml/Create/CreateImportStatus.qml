import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3

// Qt Creator does not support Namespaced enums yet
// https://bugreports.qt.io/browse/QTCREATORBUG-17850
import LocalWorkshopCreationStatus 1.0

Item {
    id: createImportStatus
    anchors.fill: parent
    state: "out"

    signal backToCreate
    signal uploadToSteamWorkshop()

    Timer {
        repeat: false
        running: true
        interval: 300
        onTriggered: {
            createImportStatus.state = "in"
        }
    }

    Connections {
        target: screenPlayCreate
        onLocalWorkshopCreationStatusChanged: {
            switch (status) {
            case LocalWorkshopCreationStatus.Started:
                txtStatus.text = "Copy video started"
                break
            case LocalWorkshopCreationStatus.CopyVideoFinished:
                txtStatus.text = "Copy Video Finished"
                break
            case LocalWorkshopCreationStatus.CopyImageFinished:
                txtStatus.text = "Copy Image Finished"
                break
            case LocalWorkshopCreationStatus.CopyConfigFinished:
                txtStatus.text = "Copy Config Finished"
                break
            case LocalWorkshopCreationStatus.ErrorCopyConfig:
                txtStatus.text = "Error Copy Config!"
                break
            case LocalWorkshopCreationStatus.Finished:
                txtStatus.text = "Success!"
                busyIndicator.running = false
                break
            default:
                break
            }
        }
    }

    RectangularGlow {
        id: effect
        anchors {
            top: wrapper.top
            left: wrapper.left
            right: wrapper.right
            topMargin: 3
        }

        height: wrapper.height
        width: wrapper.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }
    Rectangle {
        id: wrapper
        width: 800
        height: 400
        radius: 4
        anchors {
            top: parent.top
            topMargin: 100
            horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: txtStatus
            height: 80
            font.family: "Roboto"
            
            font.pixelSize: 24
            color: "gray"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors {
                top: busyIndicator.bottom
                topMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
        }

        BusyIndicator {
            id: busyIndicator
            running: true
            anchors {
                top: parent.top
                topMargin: 100
                horizontalCenter: parent.horizontalCenter
            }
        }

        Row {
            id: item1
            width: 450
            height: 49
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20

            Button {
                id: button
                text: qsTr("Create another Wallpaper")
                onClicked: {
                    backToCreate()
                    createImportStatus.state = "out"
                }
            }
            Button {
                id: button2
                text: qsTr("Upload Wallpaper to Steam")
                onClicked: {
                    uploadToSteamWorkshop()
                    createImportStatus.state = "in"
                }
            }
        }
    }
    states: [
        State {
            name: "out"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 500
                opacity: 0
            }
            PropertyChanges {
                target: effect
                color: "transparent"
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 1
            }
            PropertyChanges {
                target: effect
                color: "black"
                opacity: 0.4
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true
            SequentialAnimation {
                NumberAnimation {
                    target: wrapper
                    properties: "opacity, anchors.topMargin"
                    duration: 200
                    easing.type: Easing.OutQuart
                }
                NumberAnimation {
                    target: effect
                    properties: "opacity, color"
                    duration: 200
                    easing.type: Easing.OutQuart
                }
            }
        }
    ]
}
