import QtQuick
import QtMultimedia
import ScreenPlayWallpaper 1.0

Video {
    id: root
    volume: Wallpaper.volume
    source: Wallpaper.projectSourceFileAbsolute
    Component.onCompleted: {
        root.play()
    }
    onStopped: {
        if (Wallpaper.loops)
            root.play()
    }

    //loops: MediaPlayer.Infinite
    //        onErrorOccurred: function (error, errorString) {
    //            console.log("[qmlvideo] VideoItem.onError error " + error
    //                        + " errorString " + errorString)
    //            root.fatalError()
    //        }
}
