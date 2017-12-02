import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import LocalWorkshopCreationStatus 1.0

Item {
    id: createImportStatus
    anchors.fill: parent
    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    Connections {
        target: steamWorkshop
        onLocalWorkshopCreationStatusChanged: {
           switch(status){
           case LocalWorkshopCreationStatus.Started:
               txtStatus.text = "Copy video started"
               break;
           case LocalWorkshopCreationStatus.CopyVideoFinished:
               txtStatus.text = "Copy Video Finished"
               break;
           case LocalWorkshopCreationStatus.CopyImageFinished:
               txtStatus.text = "Copy Image Finished"
               break;
           case LocalWorkshopCreationStatus.CopyConfigFinished:
               txtStatus.text = "Copy Config Finished"
               break;
           case LocalWorkshopCreationStatus.ErrorCopyConfig:
               txtStatus.text = "Error Copy Config!"
               break;
           case LocalWorkshopCreationStatus.Finished:
               txtStatus.text = "Finished"
               busyIndicator.running = false
               break;
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
        id:wrapper
        width: 800
        height:400
        radius: 4
        anchors {
            top: parent.top
            topMargin: 100
            horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: txtStatus
            font.family: font_Roboto_Regular.name
            renderType: Text.NativeRendering
            font.pixelSize: 18
            color: "gray"
            anchors {
                top:busyIndicator.bottom
                topMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
        }

        BusyIndicator {
            id: busyIndicator
            running: true
            anchors {
                centerIn: parent
            }
        }

    }
}
