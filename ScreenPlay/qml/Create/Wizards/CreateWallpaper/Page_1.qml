import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import net.aimber.create 1.0

Item {
    id: root

    Connections {
        target: screenPlayCreate
        onCreateWallpaperStateChanged: {
            print(state)
            if (state === Create.State.ConvertingVideoFinished) {
                root.state = "finished"
            }
        }
    }

    Rectangle {
        id: wrapperProgressbar
        height: parent.height * .5
        z:1
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        Text {
            id: txtProgress
            text: Math.floor(screenPlayCreate.progress) + "%"
            font.pixelSize: 42
            color: "#626262"
            renderType: Text.NativeRendering
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: progressBar.top
                bottomMargin: 30
            }
        }

        ProgressBar {
            id: progressBar
            value: screenPlayCreate.progress
            from: 0
            to: 100
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 30
            }
        }
    }

    Rectangle {
        id: wrapperFFMPEGOutput
        color: "#eeeeee"
        radius: 3
        z:1
        anchors {
            top: wrapperProgressbar.bottom
            topMargin: 30
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        Flickable {
            anchors.fill: parent
            focus: true
            clip: true
            contentHeight: txtFFMPEG.paintedHeight
            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                policy: ScrollBar.AlwaysOn
            }
            Text {
                id: txtFFMPEG
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    margins: 20
                }
                wrapMode: Text.WordWrap
                color: "#626262"
                text: "asasas"
                renderType: Text.NativeRendering
                height: txtFFMPEG.paintedHeight
            }
        }
    }

    Item {
        id: wrapperFinished
        z:0
        anchors.fill: parent
        opacity: 0

        Text {
            id: txtDone
            text: qsTr("Video creation successful!")
            font.pixelSize: 42
            color: Material.color(Material.Green)
            renderType: Text.NativeRendering
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 30

            }
        }
    }

    Connections {
        target: screenPlayCreate
        onProcessOutput: {
            txtFFMPEG.text += text
        }
    }
    states: [
        State {
            name: "finished"
            PropertyChanges {
                target: wrapperProgressbar
                opacity:0
            }
            PropertyChanges {
                target: wrapperFFMPEGOutput
                opacity:0
            }
            PropertyChanges {
                target: wrapperFinished
                opacity:1
            }
        }
    ]
    transitions: [
        Transition {
            from: "0"
            to: "finished"
            PropertyAnimation {
                target: wrapperFFMPEGOutput
                duration: 200
                property: "opacity"
                easing.type: Easing.InOutQuad
            }
            PropertyAnimation {
                target: wrapperProgressbar
                duration: 200
                property: "opacity"
                easing.type: Easing.InOutQuad
            }
            PropertyAnimation {
                target: wrapperFinished
                duration: 200
                property: "opacity"
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
