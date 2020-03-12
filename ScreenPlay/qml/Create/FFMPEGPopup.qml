import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

import "Wizards/CreateWallpaper"

Popup {
    id: ffmpegPopup
    height: 600
    padding: 30
    width: 900

    Connections {
        target: ScreenPlay.util
        onAquireFFMPEGStatusChanged: {

            switch (aquireFFMPEGStatus) {
            case QMLUtilities.Init:
                break
            case QMLUtilities.Download:
                btnNotNow.enabled = false
                btnDownload.enabled = false
                busyIndicator.running = true
                txtStatus.text = qsTr("Begin downloading FFMPEG")
                break
            case QMLUtilities.DownloadFailed:
                btnNotNow.enabled = true
                txtStatus.text = qsTr("FFMPEG download failed")
                busyIndicator.running = false
                break
            case QMLUtilities.DownloadSuccessful:
                txtStatus.text = qsTr("FFMPEG download successful")
                break
            case QMLUtilities.Extracting:
                txtStatus.text = qsTr("Extracting FFMPEG")
                break
            case QMLUtilities.ExtractingFailedReadFromBuffer:
                btnNotNow.enabled = true
                txtStatus.text = qsTr("ERROR extracting ffmpeg from RAM")
                busyIndicator.running = false
                break
            case QMLUtilities.ExtractingFailedFFMPEG:
                btnNotNow.enabled = true
                txtStatus.text = qsTr("ERROR extracing ffmpeg")
                busyIndicator.running = false
                break
            case QMLUtilities.ExtractingFailedFFMPEGSave:
                btnNotNow.enabled = true
                txtStatus.text = qsTr("ERROR saving FFMPEG to disk")
                busyIndicator.running = false
                break
            case QMLUtilities.ExtractingFailedFFPROBE:
                btnNotNow.enabled = true
                txtStatus.text = qsTr("ERROR extracing FFPROBE")
                busyIndicator.running = false
                break
            case QMLUtilities.ExtractingFailedFFPROBESave:
                btnNotNow.enabled = true
                txtStatus.text = qsTr("ERROR saving FFPROBE to disk")
                busyIndicator.running = false
                break
            case QMLUtilities.ExtractingSuccessful:
                txtStatus.text = qsTr("Extraction successful")
                break
            case QMLUtilities.FinishedSuccessful:
                txtStatus.text = qsTr("All done and ready to go!")
                busyIndicator.running = false
                column.state = "finishedSuccessful"
                ScreenPlay.util.setFfmpegAvailable(true)
                break
            }
        }
    }

    Popup {
        id: closeDialog
        width: 400
        height: 250

        Text {
            text: qsTr("You cannot create Wallaper without FFMPEG installed!")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 16
            height: 50
            font.family: ScreenPlay.settings.font
            anchors.fill: parent
            anchors.margins: 50
            wrapMode: Text.WordWrap
            color: Material.color(Material.Orange)
        }
        RowLayout {
            height: 30
            anchors {
                right:parent.right
                bottom: parent.bottom
                left:parent.left
                margins: 20
            }

            Button {
                text: qsTr("Abort")
                onClicked: {
                    ffmpegPopup.close()
                    closeDialog.close()
                    ScreenPlay.util.setNavigation("Installed")
                }

                Layout.alignment: Qt.AlignRight
            }
            Button {
                text: qsTr("Download FFMPEG")
                highlighted: true
                onClicked: closeDialog.close()
                Layout.alignment: Qt.AlignRight
            }
        }



        anchors.centerIn: Overlay.overlay

        closePolicy: Popup.NoAutoClose
        focus: true
        modal: true
    }

    ColumnLayout {
        id: column
        anchors.fill: parent

        Text {
            id: txtDownloadFFMPEG
            text: qsTr("Before we can start creating content you need to download FFMPEG")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 16
            height: 50
            Layout.fillWidth: true
            color: "gray"
            font.family: ScreenPlay.settings.font
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#eeeeee"

            Item {
                width: parent.width * .33
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }

                Image {
                    id: imgFFMPEGLogo
                    source: "qrc:/assets/images/FFmpeg_Logo_new.svg"
                    width: sourceSize.width
                    height: sourceSize.height
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        margins: 20
                        top: parent.top
                    }
                }

                BusyIndicator {
                    id: busyIndicator
                    anchors.centerIn: parent
                    running: false
                }

                Text {
                    id: txtStatus
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 14
                    height: 50
                    Layout.fillWidth: true
                    color: Material.color(Material.Orange)
                    font.family: ScreenPlay.settings.font
                    wrapMode: Text.WordWrap
                    anchors {
                        top: busyIndicator.bottom
                        topMargin: 20
                        right: parent.right
                        left: parent.left
                    }
                }
            }

            Item {
                width: parent.width * .66
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }

                Flickable {
                    anchors.fill: parent
                    clip: true
                    contentHeight: txtExpander.paintedHeight + 100
                    contentWidth: parent.width
                    ScrollBar.vertical: ScrollBar {
                        snapMode: ScrollBar.SnapOnRelease
                        policy: ScrollBar.AlwaysOn
                    }

                    MouseArea {
                        anchors.fill: parent
                        id: maHoverEffect
                    }

                    Text {
                        id: txtExpander
                        anchors {
                            fill: parent
                            margins: 30
                            leftMargin: 0
                        }

                        color: "#626262"

                        height: txtExpander.paintedHeight
                        wrapMode: Text.WordWrap
                        font.pointSize: 12
                        font.family: ScreenPlay.settings.font
                        onLinkHovered: maHoverEffect.cursorShape = Qt.PointingHandCursor
                        onLinkActivated: Qt.openUrlExternally(link)
                        linkColor: Material.color(Material.LightBlue)
                        text: qsTr("<b>Why do we bother you with this?</b>
<br><br> Well its because of <b>copyright</b> and many many <b>patents</b>.
" + "Files like .mp4 or .webm are containers for video and audio. Every audio
" + "and video file is encoded with a certain codec. These can be open source
ce" + "and free to use like <a href='https://wikipedia.org/wiki/VP8'>VP8</a> and the newer  <a href='https://wikipedia.org/wiki/VP9'>VP9</a> (the one YouTube uses for their web
ms)" + "but there are also some proprietary ones like  <a href='https://wikipedia.org/wiki/H.264/MPEG-4_AVC'>h264</a> and the successor <a href='https://wikipedia.org/wiki/High_Efficiency_Video_Coding'>h265</a>." + "
<br>
<br>
"
+ "\n\n We as software developer now need to deal with stuff like this in a field we"
+ "do not have any expertise in. The desicion to enable only free codecs for content was"
+ "an easy one but we still need to provide a way for our user to import wallpaper "
+ "without a hassle. We do not provide  <a href='https://ffmpeg.org/'>FFMPEG</a> for "
+ "converting video and audio with ScreenPlay because we are not allowed to. We let the user download <a href='https://ffmpeg.org/'>FFMPEG</a> wich " + "is perfectly fine!
<br>
Sorry for this little inconvenience :)" + "
<br>
<br>
<center>
<b>
IF YOU DO NOT HAVE A INTERNET CONNECT YOU CAN SIMPLY PUT FFMPEG AND FFPROBE
IN THE SAME FOLDER AS YOUR SCREENPLAY EXECUTABLE!
</b>
<br>
<br>
This is usually:
<br> C:\\Program Files (x86)\\Steam\\steamapps\\common\\ScreenPlay
<br>
if you installed ScreenPlay via Steam!
</center>
<br>
<br>
" + "<b>~ Kelteseth | Elias Steurer</b>")
                    }
                }
            }
        }

        RowLayout {
            spacing: 20
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            Button {
                id: btnDownload
                text: qsTr("Download FFMPEG")
                onClicked: ScreenPlay.util.downloadFFMPEG()
                highlighted: true
                Layout.fillWidth: true
            }
            Button {
                id: btnNotNow
                text: qsTr("Not now!")
                onClicked: closeDialog.open()
                Layout.fillWidth: true
            }
        }

        states: [
            State {
                name: "finishedSuccessful"
                PropertyChanges {
                    target: column
                    opacity: 0
                    enabled: false
                }
                PropertyChanges {
                    target: successWrapper
                    opacity: 1
                    enabled: true
                }
            }
        ]

        transitions: [
            Transition {
                from: "*"
                to: "finishedSuccessful"
                PropertyAnimation {
                    target: successWrapper
                    property: "opacity"
                    duration: 250
                }
            }
        ]
    }
    Item {
        id: successWrapper
        anchors.fill: parent
        anchors.margins: 30
        opacity: 0
        enabled: false

        Text {
            id: txtDownloadFFMPEGComplete
            text: qsTr("You can now start creating content!")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 16
            height: 50
            Layout.fillWidth: true
            color: "gray"
            font.family: ScreenPlay.settings.font
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 30
            }
        }

        Image {
            id: imgOk
            fillMode: Image.PreserveAspectFit
            source: "qrc:/assets/icons/icon_done.svg"
            width: 180
            height: width
            sourceSize: Qt.size(width, width)
            anchors.centerIn: parent
            smooth: true
        }
        ColorOverlay {
            anchors.fill: imgOk
            source: imgOk
            color: Material.color(Material.LightGreen)
        }

        Button {
            text: qsTr("Start!")
            highlighted: true
            onClicked: ffmpegPopup.close()
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: imgOk.bottom
                margins: 30
            }
        }
    }
}
