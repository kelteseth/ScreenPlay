import QtQuick 2.0
import QtMultimedia
import ScreenPlayWallpaper 1.0

VideoOutput {
    id: root
    MediaPlayer {
        id: mediaPlayer
        videoOutput: root
        // volume: Wallpaper.volume
        source: Wallpaper.projectSourceFileAbsolute
        Component.onCompleted: {
            print("play", source)
            mediaPlayer.play()
        }

        // loops: MediaPlayer.Infinite
        onErrorOccurred: function (error, errorString) {
            console.log("[qmlvideo] VideoItem.onError error " + error
                        + " errorString " + errorString)
            root.fatalError()
        }
    }
}
