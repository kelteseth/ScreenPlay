import QtQuick 2.9
import QtQuick.Dialogs 1.2


Rectangle {

    property color background: "white"
    property string descriptionTitle: "value"
    property url imagePath: "qrc:/assets/icons/icon_plus.svg"
    //FIXME in 5.10 with an enum
    property bool isVideo: false
    property url externalFilePath

    id: fileDropperSingleFile
    color: fileDropperSingleFile.background
    border.color: "#C2C2C2"
    border.width: 2
    radius: 4
    state: ""

    Component.onCompleted: {
        if (isVideo) {

        } else {

        }
    }

    Image {
        id: previewImage
        anchors.fill: parent
        z: 98
        visible: false
    }




    FontLoader {
        id: font_LibreBaskerville
        source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
    }

    Item {
        id: column
        height: imageIcon.height + txtSplashInput.contentHeight
        anchors {
            right: parent.right
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        Image {
            id: imageIcon
            width: 32
            height: 32
            sourceSize.height: 32
            sourceSize.width: 32
            source: fileDropperSingleFile.imagePath
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
        }
        Text {
            id: txtSplashInput
            text: descriptionTitle
            anchors {
                top: imageIcon.bottom
                topMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            height: 40
            font.pointSize: 12
            color: "#626262"
            horizontalAlignment: Text.AlignHCenter
            font.family: font_LibreBaskerville.name
            font.italic: true
            renderType: Text.NativeRendering
        }
    }

    DropArea {
        id: dropper
        anchors.fill: parent
        onEntered: {
            fileDropperSingleFile.state = "fileEntered"
        }

        onDropped: {
            fileDropperSingleFile.state = "fileDropped"
            if (drop.hasUrls) {
                if (isVideo) {
                    if (validateVideoFileExtension(drop.urls[0])) {
                        videoPreviewLoader.setSource("CreateVideoPreviewSmall.qml",{"source":drop.urls[0]})

                    }
                } else {
                    if (validateImageFileExtension(drop.urls[0])) {
                        previewImage.source = drop.urls[0]
                        previewImage.visible = true
                    }
                }
            }
        }
        onExited: {
            fileDropperSingleFile.state = "empty"
        }

        function validateImageFileExtension(filePath) {
            var tmp = filePath.split('.').pop()
            return tmp === "png" || tmp === "jpg"
        }

        function validateVideoFileExtension(filePath) {
            var tmp = filePath.split('.').pop()
            return tmp === "vp9" || tmp === "mp4"
        }
    }
    Loader {
        id:videoPreviewLoader
        asynchronous: true
        anchors.fill: parent
        z: 97
        onLoaded: {
            //videoPreviewLoader.item.playVideo(drop.urls[0]);
        }

    }

    /* FIXME: For now only drag n drop Workshop
              because the qml file Dialog is really
              buggy and slow
    MouseArea{
        anchors.fill: parent
        onClicked: {
            if(isVideo){
                fileDialogOpenPreview.nameFilters =  ["*.mp4","GIF (*.mp4)"]
            } else {
                 fileDialogOpenPreview.nameFilters =  ["*.png *.jpg *.gif","PNG (*.png)","JPG (*.jpg)","GIF (*.gif)"]
            }

            fileDialogOpenPreview.open()
        }
    }

    FileDialog {
        id: fileDialogOpenPreview
        folder: shortcuts.home
        onAccepted: {

        }
    }*/

    states: [
        State {
            name: "fileEntered"

            PropertyChanges {
                target: fileDropperSingleFile
                color: "#d9d9d9"
            }
        },
        State {
            name: "fileDropped"

            PropertyChanges {
                target: fileDropperSingleFile
                color: "#ffbf49"
            }
        },
        State {
            name: "empty"
            PropertyChanges {
                target: fileDropperSingleFile
                color: "white"
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"

            ColorAnimation {
                duration: 200
            }
        }
    ]
}
