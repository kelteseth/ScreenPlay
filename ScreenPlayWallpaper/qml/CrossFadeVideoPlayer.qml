// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import QtQuick
import QtMultimedia

Item {
    id: root

    enum Player {
        One,
        Two
    }

    required property url source
    required property real volume
    required property bool muted
    required property string fillMode
    property bool loops: false
    property int crossFadeDuration: 500
    property bool isPlaying
    property real normalizedPosition: 0

    property int _activePlayer: CrossFadeVideoPlayer.Player.One
    property bool _initialized: false

    onNormalizedPositionChanged: {
        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            mediaPlayer1.position = normalizedPosition * mediaPlayer1.duration
        } else {
            mediaPlayer2.position = normalizedPosition * mediaPlayer2.duration
        }
    }

    onSourceChanged: {
        if (!_initialized) {
            mediaPlayer1.source = root.source
            mediaPlayer1.play()
            root._initialized = true
            return
        }

        // Stop any ongoing animations
        fadeOut.stop()

        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            // Prepare player two for transition
            mediaPlayer2.source = root.source
            
            // New video starts behind and invisible
            vo2.z = 0
            vo2.opacity = 0
            vo1.z = 1
            vo1.opacity = 1

            // Configure fade out for current (vo1), new video (vo2) stays at opacity 1 behind it
            fadeOut.target = vo1

            root._activePlayer = CrossFadeVideoPlayer.Player.Two
        } else {
            // Prepare player one for transition
            mediaPlayer1.source = source
            
            // New video starts behind and invisible
            vo1.z = 0
            vo1.opacity = 0
            vo2.z = 1
            vo2.opacity = 1

            // Configure fade out for current (vo2), new video (vo1) stays at opacity 1 behind it
            fadeOut.target = vo2

            root._activePlayer = CrossFadeVideoPlayer.Player.One
        }

        // Wait for video to be ready before starting transition
        startCrossFadeTimer.start()
    }

    onIsPlayingChanged: {
        // Only respond to the isPlaying property for controlling playback
        // after initialization
        if (_initialized) {
            if (isPlaying) {
                root._activePlayer === CrossFadeVideoPlayer.Player.One ? mediaPlayer1.play() : mediaPlayer2.play()
                if (root._activePlayer === CrossFadeVideoPlayer.Player.One) {
                    mediaPlayer1.play()
                } else if (root._activePlayer === CrossFadeVideoPlayer.Player.Two) {
                    mediaPlayer2.play()
                }
            } else {
                mediaPlayer1.pause()
                mediaPlayer2.pause()
            }
        }
    }

    onFillModeChanged: {
        let qtFillMode
        if (fillMode === "cover" || fillMode === "stretch" || fillMode === "contain") {
            qtFillMode = VideoOutput.Stretch
        } else if (fillMode === "fill") {
            qtFillMode = VideoOutput.PreserveAspectFit
        } else if (fillMode === "scale_down") {
            qtFillMode = VideoOutput.PreserveAspectCrop
        }
        vo1.fillMode = qtFillMode
        vo2.fillMode = qtFillMode
    }

    MediaPlayer {
        id: mediaPlayer1
        loops: root.loops ? MediaPlayer.Infinite : 1
        videoOutput: vo1
        audioOutput: ao
    }

    MediaPlayer {
        id: mediaPlayer2
        loops: root.loops ? MediaPlayer.Infinite : 1
        videoOutput: vo2
        audioOutput: ao
    }

    VideoOutput {
        id: vo1
        anchors.fill: parent
        opacity: 1
        z: 1
    }

    VideoOutput {
        id: vo2
        anchors.fill: parent
        opacity: 0
        z: 0
    }

    Timer {
        id: startCrossFadeTimer
        interval: 16  // Start checking immediately (one frame)
        repeat: true
        property int maxAttempts: 60  // Maximum wait time ~1 second at 60fps
        property int attempts: 0
        
        onTriggered: {
            attempts++
            
            const incomingPlayer = root._activePlayer === CrossFadeVideoPlayer.Player.One ? mediaPlayer1 : mediaPlayer2
            const incomingOutput = root._activePlayer === CrossFadeVideoPlayer.Player.One ? vo1 : vo2
            
            // Check if the incoming video has buffered frames and is actually playing
            if (incomingPlayer.hasVideo && incomingPlayer.playbackState === MediaPlayer.PlayingState) {
                // Video is ready, make it visible behind the current video and start fade
                stop()
                attempts = 0
                
                // Ensure playback
                incomingPlayer.play()
                
                // Set new video to full opacity but behind (z is already set)
                incomingOutput.opacity = 1
                
                // Fade out the current video (which is in front)
                fadeOut.start()
            } else if (attempts >= maxAttempts) {
                // Fallback: force start after timeout to prevent infinite waiting
                console.warn("Video warmup timeout, forcing crossfade")
                stop()
                attempts = 0
                
                incomingPlayer.play()
                incomingOutput.opacity = 1
                fadeOut.start()
            } else {
                // Not ready yet, ensure player is playing and continue waiting
                if (incomingPlayer.playbackState !== MediaPlayer.PlayingState) {
                    incomingPlayer.play()
                }
            }
        }
    }

    // Split the crossfade into two parallel animations
    NumberAnimation {
        id: fadeOut
        property: "opacity"
        from: 1
        to: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
        onFinished: {
            // After fade completes, stop the old player and reset z-order
            if (target === vo1) {
                mediaPlayer1.stop()
                vo1.z = 0
                vo2.z = 1
            } else {
                mediaPlayer2.stop()
                vo2.z = 0
                vo1.z = 1
            }
        }
    }

    AudioOutput {
        id: ao
        volume: root.volume
        muted: root.muted
    }
}
