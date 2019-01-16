import QtQuick 2.9
import QtQuick.Dialogs 1.2

Rectangle {
    id: fileDropperSingleFile

    property color background: "white"
    property string descriptionTitle: "value"
    property url imagePath: "qrc:/assets/icons/icon_plus.svg"
    //FIXME in 5.10 with an enum
    property bool isVideo: false
    property url externalFilePath
    property string helpText: "help"

    color: fileDropperSingleFile.background
    border.color: "#C2C2C2"
    border.width: 2
    radius: 4
    state: ""

    function reset(){
        fileDropperSingleFile.state = "empty"
        previewImage.source = ""
        videoPreviewLoader.source = ""
    }

    onStateChanged: {
        if (fileDropperSingleFile.state === "error") {
            stateChangedTimer.start()
        }
    }
    Timer {
        id: stateChangedTimer
        onTriggered: {
            fileDropperSingleFile.state = "empty"
        }
    }

    Image {
        id: previewImage
        anchors.fill: parent
        anchors.margins: 2
        z: 98
        fillMode: Image.PreserveAspectCrop
        visible: false
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
            font.family: "Libre Baskerville"
            font.italic: true
            
        }
    }
    Text {
        id: helpTextWrapper
        color: "#626262"
        anchors.fill: parent
        anchors.margins: 20
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Libre Baskerville"
        font.italic: true
        wrapMode:  Text.WordWrap
        font.pointSize: 12
        opacity: 0
        
        text: helpText
    }

    DropArea {
        id: dropper
        anchors.fill: parent
        focus: true
        onEntered: {
            fileDropperSingleFile.state = "fileEntered"
        }

        onDropped: {
            fileDropperSingleFile.state = "fileDropped"
            if (drop.hasUrls) {
                if (isVideo) {
                    if (validateVideoFileExtension(drop.urls[0])) {
                        externalFilePath = drop.urls[0]
                        videoPreviewLoader.setSource(
                                    "CreateVideoPreviewSmall.qml", {
                                        source: externalFilePath
                                    })
                    }
                } else {
                    if (validateImageFileExtension(drop.urls[0])) {
                        externalFilePath = drop.urls[0]
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
        MouseArea {
            anchors.fill: parent
            focus: true
            z: 99
            hoverEnabled: true
            onHoveredChanged: {
                if (containsMouse && videoPreviewLoader.source.toString()  === "") {
                    fileDropperSingleFile.state = "hover"
                } else {
                    fileDropperSingleFile.state = ""
                }
            }
        }
    }
    Loader {
        id: videoPreviewLoader
        asynchronous: true
        anchors.fill: parent
        anchors.margins: 2
        z: 300
    }
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
        },
        State {
            name: "error"

            PropertyChanges {
                target: fileDropperSingleFile
                color: "#ff4d4d"
            }
        },
        State {
            name: "hover"
            PropertyChanges {
                target: column
                opacity: 0
            }
            PropertyChanges {
                target: helpTextWrapper
                opacity: 1
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
            NumberAnimation {
                property: "opacity"
                duration: 200
            }
        }
    ]
}
