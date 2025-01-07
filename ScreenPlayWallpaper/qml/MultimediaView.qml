// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import QtQuick
import ScreenPlayWallpaper

Item {
    id: root
    anchors.fill: parent
    property bool fadeInDone: false

    CrossFadeVideoPlayer {
        id: videoPlayer
        anchors.fill: parent
        source: Qt.resolvedUrl(Wallpaper.projectSourceFileAbsolute)
        volume: Wallpaper.volume
        muted: Wallpaper.muted
        fillMode: Wallpaper.fillMode
        loops: Wallpaper.loops
        isPlaying: Wallpaper.isPlaying
        onIsPlayingChanged: {
            if (isPlaying && !root.fadeInDone) {
                  root.fadeInDone = true;
                  startTimer.start();
              }
        }
    }

    // Add slight delay to give the multimedia
    // engine some time to put out some frames
    Timer {
        id: startTimer
        interval: 50
        onTriggered: {
            Wallpaper.requestFadeIn();
        }
    }

    // Wait until Windows window animation is complete
    // before pausing the wallpaper
    Timer {
        id: pauseTimer
        interval: 100
        onTriggered: {
            videoPlayer.isPlaying = false;
        }
    }

    Connections {
        target: Wallpaper

        function onFillModeChanged(fillMode) {
            videoPlayer.fillMode = fillMode;
        }

        function onCurrentTimeChanged(currentTime) {
            videoPlayer.normalizedPosition = currentTime;
        }

        function onVisualsPausedChanged(visualsPaused) {
            if (!Wallpaper.isPlaying)
                return;
            if (visualsPaused)
                pauseTimer.start();
            else
                videoPlayer.isPlaying = true;
        }
    }
}
