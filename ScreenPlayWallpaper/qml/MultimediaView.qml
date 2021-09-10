import QtQuick 2.0
import QtMultimedia

VideoOutput {
    id:root
    MediaPlayer {
        id: mediaPlayer
        videoOutput: root
        //volume: Wallpaper.volume
        //source: Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
        source: Qt.resolvedUrl("C:/Users/Eli/Videos/videoplayback.webm")
        Component.onCompleted: mediaPlayer.play()
        //loops: true
        onErrorOccurred: function(error, errorString) {
            if (MediaPlayer.NoError !== error) {
                console.log("[qmlvideo] VideoItem.onError error " + error + " errorString " + errorString)
                root.fatalError()
            }
        }
    }
}
