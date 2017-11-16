import QtQuick 2.7
import QtAV 1.07

Rectangle {
    id: createVideoPreviewSmall
    color: "gray"
    property url source
    onSourceChanged: {
        player.source = Qt.resolvedUrl(createVideoPreviewSmall.source)
        print("#####" + player.error + player.source)
        player.play()
    }
    VideoOutput2 {
        id: videoOut
        anchors.fill: parent
        source: player
        fillMode: VideoOutput.Stretch
    }

    MediaPlayer {
        id: player
        videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
        loops: MediaPlayer.Infinite
        volume: 0
    }
    MouseArea{
        focus: true
        hoverEnabled: true
        onEntered: {
            player.play()
        }
        onExited: {
            player.stop()
        }
    }
}
