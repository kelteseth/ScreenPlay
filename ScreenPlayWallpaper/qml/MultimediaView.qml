import QtQuick
import QtMultimedia
import ScreenPlayWallpaper 1.0

Item {
    id: root
    anchors.fill: parent
    property bool loops: Wallpaper.loops
    property bool isPlaying: Wallpaper.isPlaying
    onIsPlayingChanged: isPlaying ? mediaPlayer.play() : mediaPlayer.pause()
    property bool isWindows: Qt.platform.os === "windows"
    signal requestFadeIn

    MediaPlayer {
        id: mediaPlayer


        source: Wallpaper.projectSourceFileAbsolute
        Component.onCompleted: {
            mediaPlayer.play()
            root.requestFadeIn()
        }
        loops: !root.isWindows && root.loops ? MediaPlayer.Infinite : 1
        onPositionChanged: {
            if (!root.isWindows)
                return

            if ((mediaPlayer.position >= mediaPlayer.duration) && root.loops) {
                mediaPlayer.setPosition(0)
                mediaPlayer.stop()
                mediaPlayer.play()
            }
        }

        videoOutput: vo
        audioOutput: ao
    }
    VideoOutput {
        id: vo
        anchors.fill: parent
    }

    AudioOutput {
        id: ao
        volume: Wallpaper.volume
    }
}
