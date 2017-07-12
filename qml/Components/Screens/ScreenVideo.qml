import QtQuick 2.7
import QtAV 1.07


Rectangle {
    color: "gray"
    visible: true

    Video {
        id: video
        anchors.fill: parent
        videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
        onPlaybackStateChanged: {

        }

        Component.onCompleted: {
            //video.source = Qt.resolvedUrl("file:///" + wallpaper.absoluteFilePath.toString())
            //video.play()
        }
        onStopped: {
            //video.play()
        }


    }

}
