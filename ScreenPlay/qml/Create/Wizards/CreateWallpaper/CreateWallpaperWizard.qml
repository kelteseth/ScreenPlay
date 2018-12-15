import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3
import net.aimber.create 1.0

Item {
    id: createNew
    anchors.fill: parent
    state: "out"

    property string filePath
    property bool canNext: false

    Component.onCompleted: {
        state = "in"
        utility.setNavigationActive(false)
    }

    //Blocks some MouseArea from create page
    MouseArea {
        anchors.fill: parent
    }

    Connections {
        target: screenPlayCreate
        onCreateWallpaperStateChanged: {

            if (state === Create.State.ConvertingPreviewGifError
                    || state === Create.State.ConvertingPreviewVideoError
                    || state === Create.State.ConvertingPreviewImageError
                    || state === Create.State.AnalyseVideoError) {
                createNew.state = "error"
                return
            }
        }
    }

    Timer {
        interval: 1000
        triggeredOnStart: false
        running: true
        repeat: false
        onTriggered: {
            screenPlayCreate.createWallpaperStart(filePath)
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

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 0
        }

        width: 910
        radius: 4
        height: 460

        Item {
            id: wrapperContent
            z: 10
            anchors.fill: parent

            Text {
                id: txtHeadline
                text: qsTr("Convert a video to a wallpaper")
                height: 40
                font.family: "Roboto"
                font.weight: Font.Light
                color: "#757575"
                renderType: Text.NativeRendering
                font.pixelSize: 23
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 40
                    bottomMargin: 0
                }
            }

            Item {
                id: wrapperLeft
                width: parent.width * .5
                anchors {
                    left: parent.left
                    top: txtHeadline.bottom
                    margins: 30
                    bottom: parent.bottom
                }

                Rectangle {
                    id: imgWrapper
                    width: 425
                    height: 247
                    anchors {
                        top: parent.top
                        left: parent.left
                    }

                    color: Material.color(Material.Grey)

                    AnimatedImage {
                        id: imgPreview
                        asynchronous: true
                        playing: true
                        visible: false
                        anchors.fill: parent
                    }

                    BusyIndicator {
                        id: busyIndicator
                        anchors.centerIn: parent
                        running: true
                    }

                    Text {
                        id: txtConvertNumber
                        color: "white"
                        text: qsTr("")
                        font.pixelSize: 21
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottom: parent.bottom
                            bottomMargin: 40
                        }
                    }

                    Text {
                        id: txtConvert
                        color: "white"
                        text: qsTr("Generating preview video...")
                        font.pixelSize: 14
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottom: parent.bottom
                            bottomMargin: 20
                        }
                    }

                    Connections {
                        target: screenPlayCreate

                        onCreateWallpaperStateChanged: {
                            if (state === Create.State.ConvertingPreviewImageFinished) {
                                imgPreview.source = "file:///"
                                        + screenPlayCreate.workingDir + "/preview.png"
                                imgPreview.visible = true
                                txtConvert.text = qsTr(
                                            "Converting Video preview mp4")
                            }

                            if (state === Create.State.ConvertingPreviewVideo) {
                                txtConvert.text = qsTr(
                                            "Generating preview video...")
                            }

                            if (state === Create.State.ConvertingPreviewGif) {
                                txtConvert.text = qsTr(
                                            "Generating preview gif...")
                            }

                            if (state === Create.State.ConvertingPreviewGifFinished) {
                                imgPreview.source = "file:///"
                                        + screenPlayCreate.workingDir + "/preview.gif"
                                imgPreview.visible = true
                                imgPreview.playing = true
                            }
                            if (state === Create.State.ConvertingAudio) {
                                txtConvert.text = qsTr("Converting Audio...")
                            }
                            if (state === Create.State.ConvertingVideo) {
                                txtConvert.text = qsTr("Converting Video...")
                            }

                            if (state === Create.State.Finished) {
                                imgSuccess.source = "file:///"
                                        + screenPlayCreate.workingDir + "/preview.gif"
                            }
                        }
                        onProgressChanged: {
                            var percentage = Math.floor(progress * 100)
                            if (percentage > 100)
                                percentage = 100
                            txtConvertNumber.text = percentage + "%"
                        }
                    }
                }
                RowLayout {
                    id: row
                    height: 50
                    anchors {
                        top: imgWrapper.bottom
                        topMargin: 20

                        right: parent.right
                        rightMargin: 30
                        left: parent.left
                    }

                    Rectangle {
                        height: 50
                        color: "#eeeeee"
                        Layout.fillWidth: true

                        Text {
                            id: txtCustomPreviewPath
                            color: "#333333"
                            text: qsTr("Add custom preview image")
                            anchors {
                                verticalCenter: parent.verticalCenter
                                left: parent.left
                                leftMargin: 10
                            }
                        }

                        Button {
                            id: button
                            Material.background: Material.Orange
                            Material.foreground: "white"
                            text: qsTr("Choose Image")
                            anchors {
                                right: parent.right
                                rightMargin: 10
                            }
                            onClicked: fileDialogOpenFile.open()
                        }


                        FileDialog {
                            id: fileDialogOpenFile
                            nameFilters: ["*.png *.jpg"]
                            onAccepted: {
                                var file = fileDialogOpenFile.file.toString()

                                txtCustomPreviewPath.text = fileDialogOpenFile.file
                            }
                        }
                    }
                }
            }
            Item {
                id: wrapperRight
                width: parent.width * .5
                anchors {
                    top: txtHeadline.bottom
                    topMargin: 30
                    bottom: parent.bottom
                    right: parent.right
                }

                ColumnLayout {
                    id: column
                    spacing: 0
                    anchors {

                        right: parent.right
                        left: parent.left
                        margins: 30
                        top:parent.top
                        topMargin: 0
                        bottom: column1.top
                        bottomMargin: 50
                    }

                    TextField {
                        id: textField
                        placeholderText: qsTr("Name")
                        width:parent.width
                        Layout.fillWidth: true
                        onTextChanged: {
                            if (textField.text.length >= 3) {
                                canNext = true
                            } else {
                                canNext = false
                            }
                        }
                    }

                    TextField {
                        id: textField1
                        placeholderText: qsTr("Description")
                        width:parent.width
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: textField2
                        placeholderText: qsTr("Youtube URL")
                        width:parent.width
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: textField3
                        width:parent.width
                        placeholderText: qsTr("Tags")
                        Layout.fillWidth: true
                    }
                }

                Row {
                    id: column1
                    height: 80
                    width: childrenRect.width
                    spacing: 10
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        bottom: parent.bottom
                    }

                    Button {
                        id: btnExit
                        text: qsTr("Abort")
                        Material.background: Material.Gray
                        Material.foreground: "white"
                        onClicked: {
                            screenPlayCreate.abort()
                            utility.setNavigationActive(true)
                            utility.setNavigation("Create")
                        }
                    }

                    NextButton {
                        id: btnFinish
                        onClicked: {
                            if (btnFinish.state === "enabled" && canNext) {
                                screenPlayCreate.createWallpaperProjectFile(
                                            textField.text, textField1.text)
                                utility.setNavigationActive(true)
                                createNew.state = "success"
                            }
                        }
                    }
                }

                Connections {
                    target: screenPlayCreate
                    onCreateWallpaperStateChanged: {
                        if (state === Create.State.ConvertingVideoFinished) {
                            btnFinish.state = "enabled"
                        }
                    }
                }
            }
        }

        CreateWallpaperError {
            id: wrapperError
            anchors.fill: parent
        }

        CreateWallpaperSuccess {
            id: wrapperSuccess
            anchors.fill: parent
        }

        MouseArea {
            anchors {
                top: parent.top
                right: parent.right
                margins: 5
            }
            width: 32
            height: width
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                createNew.state = "out"
                timerBack.start()
            }

            Image {
                id: imgClose
                source: "qrc:/assets/icons/font-awsome/close.svg"
                width: 16
                height: 16
                anchors.centerIn: parent
                sourceSize: Qt.size(width, width)
            }
            ColorOverlay {
                id: iconColorOverlay
                anchors.fill: imgClose
                source: imgClose
                color: "gray"
            }
            Timer {
                id: timerBack
                interval: 800
                onTriggered: {
                    screenPlayCreate.abort()
                    utility.setNavigationActive(true)
                    utility.setNavigation("Create")
                }
            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 800
                opacity: 0
            }
            PropertyChanges {
                target: effect
                opacity: 0
            }
            PropertyChanges {
                target: wrapperError
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
                opacity: .4
            }
            PropertyChanges {
                target: wrapperError
                opacity: 0
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: wrapperContent
                opacity: 0
                z: 0
            }
            PropertyChanges {
                target: wrapperError
                opacity: 1
            }
        },
        State {
            name: "success"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: wrapperContent
                opacity: 0
                z: 0
            }
            PropertyChanges {
                target: wrapperSuccess
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            SequentialAnimation {

                PauseAnimation {
                    duration: 400
                }
                ParallelAnimation {

                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "anchors.topMargin"
                        easing.type: Easing.OutQuart
                    }
                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "opacity"
                        easing.type: Easing.OutQuart
                    }
                    SequentialAnimation {

                        PauseAnimation {
                            duration: 1000
                        }
                        PropertyAnimation {
                            target: effect
                            duration: 300
                            property: "opacity"
                            easing.type: Easing.OutQuart
                        }
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "out"

            ParallelAnimation {

                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "anchors.topMargin"
                    easing.type: Easing.OutQuart
                }
                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
                SequentialAnimation {

                    PauseAnimation {
                        duration: 500
                    }
                    PropertyAnimation {
                        target: effect
                        duration: 300
                        property: "opacity"
                        easing.type: Easing.OutQuart
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "error"
            SequentialAnimation {
                PropertyAnimation {
                    target: wrapperContent
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
                PauseAnimation {
                    duration: 50
                }
                PropertyAnimation {
                    target: wrapperError
                    duration: 200
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
            }
        },
        Transition {
            from: "in"
            to: "success"
            SequentialAnimation {
                PropertyAnimation {
                    target: wrapperContent
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
                PauseAnimation {
                    duration: 50
                }
                PropertyAnimation {
                    target: wrapperSuccess
                    duration: 200
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
            }
        }
    ]
}












/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
