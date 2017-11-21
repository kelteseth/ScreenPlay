import QtQuick 2.7
import QtQuick.Dialogs.qml 1.0
import QtQuick.Controls 2.2

Item {
    id: createUpload
    state: "invisible"
    onStateChanged: print(createUpload.state)

    signal uploadCompleted

    Rectangle {
        opacity: .9
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onClicked: {

            }
        }
    }

    Connections {
        target: steamWorkshop
        onWorkshopCreationCopyVideo: {
            print("Copy video", sucessful)
        }
        onWorkshopCreationCopyImage: {
            print("Copy image", sucessful)
        }
        onWorkshopCreationCompleted: {
            print("Workshop Creation Complete", sucessful)
        }
        onLocalFileCopyCompleted: {
            print("Copy complete", sucessful)
        }
        onWorkshopCreationFolderDuplicate: {
            print("duplicate")
        }
    }

    function startUpload() {
        timerUpload.start()
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    Rectangle {
        id: steamUploadWrapper
        color: "white"
        width: 900
        opacity: 0
        visible: false
        height: 600
        anchors.margins: 10
        radius: 4
        z: 98
        anchors.centerIn: parent

        ProgressBar {
            id: progressBar
            from: 0
            to: steamWorkshop.bytesTotal
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            value: steamWorkshop.itemProcessed
        }

        Timer {
            id: timerUpload
            running: false
            triggeredOnStart: true
            repeat: true
            interval: 500
            onRunningChanged: print(timerUpload.running)
            onTriggered: {
                var status = steamWorkshop.getItemUpdateProcess()
                print(status)
                switch (status) {
                case 0:
                    text2.text = "The item update handle was invalid, the job might be finished. Who knows..."
                    break
                case 1:
                    text2.text = "The item update is processing configuration data."
                    break
                case 2:
                    text2.text = "The item update is reading and processing content files."
                    break
                case 3:
                    text2.text = "The item update is uploading content changes to Steam."
                    break
                case 4:
                    text2.text = "The item update is uploading new preview file image."
                    break
                case 5:
                    text2.text = "The item update is committing all changes."
                    timerUpload.running = false
                    uploadCompleted()
                    break
                default:
                    break
                }
            }
        }

        Text {
            id: text1
            text: steamWorkshop.itemProcessed + " / " + steamWorkshop.bytesTotal
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: progressBar.top
            anchors.bottomMargin: 10
            font.pointSize: 16
            renderType: Text.NativeRendering
            font.family: font_Roboto_Regular.name
        }

        Text {
            id: text2
            text: qsTr("")
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: progressBar.bottom
            anchors.topMargin: 10
            font.pointSize: 16
            renderType: Text.NativeRendering
            font.family: font_Roboto_Regular.name
        }
    }

    Rectangle {
        id: localImportWrapper
        color: "#AAffffff"
        anchors.fill: parent
        opacity: 0
        onOpacityChanged: print(opacity)
        visible: false
    }

    states: [
        State {
            name: "invisible"

            PropertyChanges {
                target: createUpload
                opacity: 0
                visible: false
            }
        },

        State {
            name: "error"
        },
        State {
            name: "import"
            PropertyChanges {
                target: localImportWrapper
                opacity: 1
                visible: true
            }
        },
        State {
            name: "upload"
            PropertyChanges {
                target: steamUploadWrapper
                opacity: 1
                visible: true
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"

            NumberAnimation {
                target: steamUploadWrapper
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
