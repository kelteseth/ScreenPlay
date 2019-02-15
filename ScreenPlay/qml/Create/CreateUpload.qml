import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import RemoteWorkshopCreationStatus 1.0


Item {
    id: createUpload
    anchors.fill: parent
    state: "out"
    Component.onCompleted: state = "in"

    property bool isVideoPlaying: true
    property url videoFile: ""
    property string projectFile
    property var jsonProjectFile



    // First we parse the content of the project file
    // TODO: Implement parse error
    onProjectFileChanged: {
        jsonProjectFile = JSON.parse(screenPlaySettings.loadProject(
                                         projectFile))
    }

    // Now we add the content
    onJsonProjectFileChanged: {
        txtTitle.text = jsonProjectFile.title
        txtDescription.text = jsonProjectFile.description

        var newStr = projectFile.substring(0, projectFile.length - 12)

        imgPreview.source = Qt.resolvedUrl(newStr + jsonProjectFile.preview)
    }

    Connections {
        target: steamWorkshop
        ignoreUnknownSignals: true
        onWorkshopItemCreated: {
            print(userNeedsToAcceptWorkshopLegalAgreement,
                  publishedFileId + "" + projectFile)
            steamWorkshop.submitWorkshopItem(txtTitle.text.toString(),
                                             txtDescription.text.toString(),
                                             "english", 0, projectFile,
                                             videoFile, publishedFileId)
        }
        onRemoteWorkshopCreationStatusChanged: {
            switch (status) {
            case RemoteWorkshopCreationStatus.Started:
                timerUpload.start()
                break
            case RemoteWorkshopCreationStatus.ErrorUnknown:
                timerUpload.stop()
                break
            }
        }
    }

    Timer {
        id: timerUpload
        running: false
        repeat: true
        interval: 100
        onTriggered: {
            var status = steamWorkshop.getItemUpdateProcess()
            print(status + " -  " + steamWorkshop.itemProcessed + " / " + steamWorkshop.bytesTotal)
            switch (status) {
            case 0:
                txtUploadStatus.text = "0. The item update handle was invalid, the job might be finished. Who knows..."
                break
            case 1:
                txtUploadStatus.text = "1. The item update is processing configuration data."
                pbUpload.indeterminate = true
                break
            case 2:
                txtUploadStatus.text = "2. The item update is reading and processing content files."
                pbUpload.indeterminate = false
                txtUploadSize.text = steamWorkshop.itemProcessed + "/" + steamWorkshop.bytesTotal
                pbUpload.value = steamWorkshop.itemProcessed / steamWorkshop.bytesTotal
                break
            case 3:
                txtUploadStatus.text = "3. The item update is uploading content changes to Steam."
                if (steamWorkshop.itemProcessed === 0) {
                    if (!pbUpload.indeterminate)
                        pbUpload.indeterminate = true
                } else {
                    if (pbUpload.indeterminate)
                        pbUpload.indeterminate = false

                    txtUploadSize.text = steamWorkshop.itemProcessed + "/"
                            + steamWorkshop.bytesTotal
                    pbUpload.value = steamWorkshop.itemProcessed / steamWorkshop.bytesTotal
                }
                break
            case 4:
                txtUploadStatus.text = "4. The item update is uploading new preview file image."
                pbUpload.indeterminate = true
                pbUpload.value = 1
                txtUploadSize.text = ""
                break
            case 5:
                txtUploadStatus.text = "5. Done!"
                createUpload.state = "uploadComplete"
                //globalNavigationHelper.setNavigation("Create")
                break
            default:
                break
            }
        }
    }

    RectangularGlow {
        id: effect
        anchors {
            top: videoOutWrapper.top
            left: videoOutWrapper.left
            right: videoOutWrapper.right
            topMargin: 3
        }

        height: videoOutWrapper.height
        width: videoOutWrapper.width
        cached: true
        z: 9
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Rectangle {
        id: videoOutWrapper
        width: 450
        height: 263
        z: 12
        color: "gray"
        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }

        Image {
            id: imgPreview
            anchors.fill: parent
        }

        Rectangle {
            id: rectProgressBar
            height: 5
            color: "orange"
            anchors {
                right: parent.right
                rightMargin: videoOutWrapper.width
                left: parent.left
                leftMargin: 0
                bottom: parent.bottom
                bottomMargin: 0
            }
        }
    }

    RectangularGlow {
        id: effect2
        anchors {
            top: contentWrapper.top
            left: contentWrapper.left
            right: contentWrapper.right
            topMargin: 3
        }

        height: contentWrapper.height
        width: contentWrapper.width
        cached: true
        z: 9
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Rectangle {
        id: contentWrapper
        width: 800
        height: 350
        z: 10
        radius: 4
        anchors {
            top: parent.top
            topMargin: 250
            horizontalCenter: parent.horizontalCenter
        }

        ColumnLayout {

            anchors {
                top: parent.top
                topMargin: 55
                right: parent.right
                rightMargin: 78
                bottom: parent.bottom
                bottomMargin: 71
                left: parent.left
                leftMargin: 78
            }

            TextField {
                id: txtTitle
                height: 60
                Layout.preferredWidth: parent.width
                selectByMouse: true
                placeholderText: "Title"
            }
            TextField {
                id: txtDescription
                height: 60
                Layout.preferredWidth: parent.width
                selectByMouse: true
                placeholderText: "Description"
            }

            TextField {
                id: txtYoutube
                height: 60
                Layout.preferredWidth: parent.width
                selectByMouse: true
                placeholderText: "Youtube Preview URL"
            }
            Row {
                height: 50
                Layout.alignment: Qt.AlignHCenter
                Item {
                    width: 100
                    height: 50
                    CheckBox {
                        text: qsTr("Videos")
                    }
                }
                Item {
                    width: 100
                    height: 50
                    CheckBox {
                        text: qsTr("Scenes")
                    }
                }
                Item {
                    width: 100
                    height: 50
                    CheckBox {
                        text: qsTr("Widgets")
                    }
                }
                Item {
                    width: 100
                    height: 50
                    CheckBox {
                        text: qsTr("AppDrawer")
                    }
                }
            }
        }

        Button {
            text: qsTr("Upload to Steam")
            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            Material.background: Material.Orange
            Material.foreground: "white"
            icon.source: "qrc:/assets/icons/icon_upload.svg"
            icon.color: "white"
            icon.width: 16
            icon.height: 16
            onClicked: {
                createUpload.state = "upload"
                // We need to first create a workshop item
                // after we received the new item handle
                // from steam we can upload the content
                steamWorkshop.createWorkshopItem()
            }
        }
    }

    Rectangle {
        id: uploadWrapper
        opacity: 0
        width: 600
        height: 120
        radius: 4
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: videoOutWrapper.bottom
            topMargin: 30
        }

        Text {
            id: txtUploadSize
            color: "gray"
            font.pixelSize: 16
            height: 30
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 20
            }
            font.family: "Roboto"

        }
        Text {
            id: txtUploadStatus
            text: qsTr("Creating Workshop Item")
            color: "gray"
            font.pixelSize: 16
            height: 30
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: txtUploadSize.bottom
                topMargin: 20
            }
            font.family: "Roboto"

        }

        ProgressBar {
            id: pbUpload
            value: .1
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: txtUploadStatus.bottom
            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: createUpload
                opacity: 0
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 0
                opacity: 0
                anchors.topMargin: 50
            }
            PropertyChanges {
                target: effect
                opacity: 0
                color: "transparent"
            }
            PropertyChanges {
                target: contentWrapper
                anchors.topMargin: -500
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: createUpload
                opacity: 1
            }

            PropertyChanges {
                target: effect
                opacity: .4
                color: "black"
            }
            PropertyChanges {
                target: contentWrapper
                opacity: 1
                anchors.topMargin: 250
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 12
                opacity: 1
                anchors.topMargin: 20
            }
            PropertyChanges {
                target: uploadWrapper
                opacity: 0
            }
        },
        State {
            name: "upload"
            PropertyChanges {
                target: createUpload
                opacity: 1
            }

            PropertyChanges {
                target: effect
                opacity: 0
                color: "transparent"
            }
            PropertyChanges {
                target: contentWrapper
                opacity: 1
                anchors.topMargin: -500
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 12
                opacity: 1
                anchors.topMargin: 150
            }
            PropertyChanges {
                target: uploadWrapper
                opacity: 1
            }
        },
        State {
            name: "uploadComplete"
            PropertyChanges {
                target: createUpload
                opacity: 1
            }

            PropertyChanges {
                target: effect
                opacity: 0
                color: "transparent"
            }
            PropertyChanges {
                target: contentWrapper
                opacity: 1
                anchors.topMargin: -500
            }
            PropertyChanges {
                target: videoOutWrapper
                z: 12
                opacity: 1
                anchors.topMargin: 150
            }
            PropertyChanges {
                target: uploadWrapper
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true

            SequentialAnimation {
                PauseAnimation {
                    duration: 500
                }
                ParallelAnimation {
                    PropertyAnimation {
                        duration: 300
                        target: contentWrapper
                        property: "opacity"
                    }
                    PropertyAnimation {
                        duration: 300
                        target: contentWrapper
                        property: "anchors.topMargin"
                        easing.type: Easing.OutQuart
                    }
                }
                ParallelAnimation {
                    PropertyAnimation {
                        duration: 250
                        target: videoOutWrapper
                        property: "opacity"
                    }
                    PropertyAnimation {
                        duration: 250
                        target: effect
                        properties: "color, opacity"
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "upload"
            reversible: true

            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation {
                        duration: 300
                        target: contentWrapper
                        property: "opacity"
                    }
                    PropertyAnimation {
                        duration: 300
                        target: contentWrapper
                        property: "anchors.topMargin"
                        easing.type: Easing.OutQuart
                    }
                }
                ParallelAnimation {
                    PropertyAnimation {
                        duration: 250
                        target: videoOutWrapper
                        property: "topMargin"
                    }
                    PropertyAnimation {
                        duration: 250
                        target: effect
                        properties: "color, opacity"
                    }
                    PropertyAnimation {
                        duration: 250
                        target: uploadWrapper
                        property: "opacity"
                    }
                }
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
