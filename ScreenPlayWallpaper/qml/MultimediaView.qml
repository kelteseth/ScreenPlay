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
        volume: Wallpaper.currentState.volume
        targetVolume: Wallpaper.targetState.volume
        muted: Wallpaper.currentState.muted
        fillMode: Wallpaper.currentState.fillMode
        loops: Wallpaper.currentState.loops
        isPlaying: Wallpaper.currentState.isPlaying
        onIsPlayingChanged: {
            if (isPlaying && !root.fadeInDone) {
                root.fadeInDone = true
                startTimer.start()
            }
        }
        onTransitionFinished: {
            // Apply target settings (volume, fillMode) after crossfade completes
            Wallpaper.applyTargetSettings()
        }
    }

    // Add slight delay to give the multimedia
    // engine some time to put out some frames
    Timer {
        id: startTimer
        interval: 50
        onTriggered: {
            Wallpaper.requestFadeIn()
        }
    }

    // Wait until Windows window animation is complete
    // before pausing the wallpaper
    Timer {
        id: pauseTimer
        interval: 100
        onTriggered: {
            videoPlayer.isPlaying = false
        }
    }

    Connections {
        target: Wallpaper.currentState

        function onFillModeChanged(fillMode) {
            videoPlayer.fillMode = fillMode
        }

        function onVisualsPausedChanged(visualsPaused) {
            if (!Wallpaper.currentState.isPlaying)
                return
            if (visualsPaused)
                pauseTimer.start()
            else
                videoPlayer.isPlaying = true
        }
    }
}
