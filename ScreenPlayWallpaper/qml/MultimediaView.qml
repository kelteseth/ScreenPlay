import QtQuick
import QtMultimedia
import ScreenPlayWallpaper

Item {
    id: root
    anchors.fill: parent
    property bool loops: Wallpaper.loops
    property bool isPlaying: Wallpaper.isPlaying
    property string fillMode: Wallpaper.fillMode
    property bool fadeInDone: false
    onFillModeChanged: {
        // Convert Web based video modes to the limited Qt fillModes
        if (fillMode === "cover" || fillMode === "stretch" || fillMode === "contain") {
            return vo.fillMode = VideoOutput.Stretch;
        }
        if (fillMode === "fill") {
            return vo.fillMode = VideoOutput.PreserveAspectFit;
        }
        if (fillMode === "scale_down") {
            return vo.fillMode = VideoOutput.PreserveAspectCrop;
        }
    }

    onIsPlayingChanged: isPlaying ? mediaPlayer.play() : mediaPlayer.pause()
    property bool isWindows: Qt.platform.os === "windows"

    property string source: Wallpaper.projectSourceFileAbsolute
    onSourceChanged: {
        // Qt 6.3 workaround
        mediaPlayer.stop();
        mediaPlayer.source = Qt.resolvedUrl(root.source);
        mediaPlayer.play();
    }

    // Add slight delay to give the multimedia
    // engine some time to put out some frames. This
    // fixes some white frame flickering
    Timer {
        id: startTimer
        interval: 50
        onTriggered: {
            Wallpaper.requestFadeIn();
        }
    }

    MediaPlayer {
        id: mediaPlayer
        onPlaybackStateChanged: {
            if (mediaPlayer.playbackState == MediaPlayer.PlayingState && !fadeInDone) {
                fadeInDone = true;
                startTimer.start();
            }
        }
        loops: root.loops ? MediaPlayer.Infinite : 1
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
        muted: Wallpaper.muted
    }
    // Wait until Windows window animation is complete
    // before pausing the wallpaper
    Timer {
        id: pauseTimer
        interval: 100
        onTriggered: {
            mediaPlayer.pause();
        }
    }
    Connections {
        function onFillModeChanged(fillMode) {
            if (fillMode === "stretch") {
                vo.fillMode = VideoOutput.Stretch;
                return;
            }
            if (fillMode === "fill") {
                vo.fillMode = VideoOutput.PreserveAspectFit;
                return;
            }
            if (fillMode === "contain" || fillMode === "cover" || fillMode === "scale-down") {
                vo.fillMode = VideoOutput.PreserveAspectCrop;
            }
        }

        function onCurrentTimeChanged(currentTime) {
            mediaPlayer.position = currentTime * mediaPlayer.duration;
        }

        function onVisualsPausedChanged(visualsPaused) {
            if (!Wallpaper.isPlaying)
                return;
            if (visualsPaused)
                pauseTimer.start();
            else
                mediaPlayer.play();
        }

        target: Wallpaper
    }
}
