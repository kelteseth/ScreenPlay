import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3
import net.aimber.create 1.0

Item {
    id: wrapperContent
    Text {
        id: txtHeadline
        text: qsTr("Convert a video to a wallpaper")
        height: 40
        font.family: "Roboto"
        font.weight: Font.Light
        color: "#757575"

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
