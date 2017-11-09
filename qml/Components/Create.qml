import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.0

CustomPage {
    id: create
    pageName: ""
    state: "init"

    Connections {
        target: steamWorkshop
        onWorkshopItemCreated: {
            if (userNeedsToAcceptWorkshopLegalAgreement) {

                //TODO!!
            } else {

            }
        }
        onLocalFileCopyCompleted: {
            create.state = "init"
        }
    }

    Connections {
        target: createUpload
        onUploadCompleted:{
            create.state = "init"
        }
    }

    Connections {
        target: leftArea
        onHasEmptyField: {
            if (fieldNumber === 0) {
                fileDropperVideo.state = "error"
            } else if (fieldNumber === 1) {
                fileDropperPreview.state = "error"
            }
        }
        onCreateLocalWallpaperStarted: {
            create.state = "createLocalWallpaper"
        }
        onCreateSteamWallpaperStarted: {
            create.state = "upload"
            createUpload.startUpload()
        }
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    FontLoader {
        id: font_LibreBaskerville
        source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
    }

    CreateLeftArea {
        id: leftArea
        width: parent.width * .35
        radius: 4
        border.width: 2
        border.color: "#c2c2c2"
        anchors.leftMargin: 30
        anchors.topMargin: 30
        anchors {
            top: parent.top
            left: parent.left
            margins: 10
        }
    }

    Item {
        id: rightArea
        width: parent.width * .58
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: 30
        }
        Item {
            id: rightTop
            height: 200
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
            Item {
                width: parent.width * .48
                height: parent.height

                FileDropperSingleFile {
                    id: fileDropperVideo
                    anchors.fill: parent
                    z: 99
                    descriptionTitle: "Drop your video here"
                    helpText: "Supportet Video formats: mp4 for older devices and VP9 for newer like NVidia 1000 or AMD VEGA"
                    isVideo: true
                    imagePath: "qrc:/assets/icons/icon_tv.svg"
                    onExternalFilePathChanged: leftArea.videoPath
                                               = fileDropperVideo.externalFilePath
                }
            }
            Item {
                width: parent.width * .48
                anchors.right: parent.right
                height: parent.height

                FileDropperSingleFile {
                    id: fileDropperPreview
                    anchors.fill: parent
                    z: 99
                    descriptionTitle: "Set Preview Image"
                    helpText: "Use PNG for best results. The image ratio should be 21:9. Preffered resulution 123x123."
                    imagePath: "qrc:/assets/icons/icon_single_image.svg"
                    onExternalFilePathChanged: leftArea.previewPath
                                               = fileDropperPreview.externalFilePath
                }
            }

            Item {
                id: rightBottom
                height: 300
                anchors {
                    top: rightTop.bottom
                    topMargin: 30
                    right: parent.right
                    left: parent.left
                }
                FileDropperSingleFile {
                    anchors.fill: parent
                    z: 99
                    descriptionTitle: "Add additional images"
                    imagePath: "qrc:/assets/icons/icon_multiple_images.svg"
                }
            }
        }

    }

    CreateUpload {
        id: createUpload
        anchors.fill: parent
        z: -1
        opacity: 0
        visible: false
    }

    states: [
        State {
            name: "init"
            PropertyChanges {
                target: createUpload
                z: -1
                opacity: 0
                visible: false
            }
        },
        State {
            name: "upload"
            PropertyChanges {
                target: createUpload
                z: 99
                opacity: 1
                visible: true
            }
        }
    ]
    transitions: [
        Transition {
            from: "init"
            to: "upload"
            PropertyAnimation {
                properties: "opacity"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
