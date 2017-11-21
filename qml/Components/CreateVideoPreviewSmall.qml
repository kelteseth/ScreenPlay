import QtQuick 2.7
import QtAV 1.07

Rectangle {
    id: createVideoPreviewSmall
    visible: true
    property url source
    anchors.fill: parent
    Component.onCompleted: {

    }

    onSourceChanged: {
        var str = createVideoPreviewSmall.source
        str.toString().replace(/\\/g, "\\\\")
        player.source = Qt.resolvedUrl(str)
        print(player.source)
        player.play()
    }
    VideoOutput2 {
        id: videoOut
        anchors.fill: parent
        opengl: true
        source: player
        fillMode: VideoOutput.Stretch
        z:311
    }

    MediaPlayer {
        id: player
        videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
        loops: MediaPlayer.Infinite
        volume: 1
        //source: Qt.resolvedUrl("C:\\Users\\Eli\\AppData\\Local\\Aimber\\ScreenPlay\\Wallpaper\\786361026\\a.mp4")
        onSourceChanged: {

        }

        onPlaying: print("playing")
        onError: {
            print(errorString)
        }

    }
    MouseArea{
        focus: true
        hoverEnabled: true
        onEntered: {
            print("play")
            player.play()
        }
        onExited: {
            print("stop")
            player.stop()
        }
    }
}
