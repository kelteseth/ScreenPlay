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
    required property real volume        // Current/outgoing volume
    required property real targetVolume  // Volume for incoming video during transition
    required property bool muted
    required property string fillMode
    property bool loops: false
    property int crossFadeDuration: 500
    property bool isPlaying
    property real normalizedPosition: 0

    // Target render rate in fps (0 = unlimited). For video the decoder runs on
    // the media clock, so the window-level frame limiter cannot slow it down.
    // Instead we slow playback: at half a clip's native fps the decoder only
    // has to produce half as many frames per wall-second, which actually lowers
    // decode power. The trade-off is slow-motion, so this is only sensible for
    // ambient wallpapers where absolute motion speed does not matter.
    property int fpsLimit: 0

    // Never slow below this rate: extreme ratios (e.g. 1 fps on a 60 fps clip)
    // look frozen and the backend paces them poorly. Below the floor the decode
    // saving plateaus while the window frame limiter still drops presented
    // frames to the requested fps.
    readonly property real _minPlaybackRate: 0.1

    onFpsLimitChanged: {
        _applyPlaybackRate(mediaPlayer1)
        _applyPlaybackRate(mediaPlayer2)
    }

    // native fps comes from the clip's metadata and is only valid once it has
    // loaded, so this is also called from each player's onMetaDataChanged.
    function _applyPlaybackRate(player) {
        if (!player)
            return
        let rate = 1.0
        if (root.fpsLimit > 0) {
            const nativeFps = player.metaData.value(MediaMetaData.VideoFrameRate)
            if (nativeFps && nativeFps > 0)
                rate = Math.max(root._minPlaybackRate, Math.min(1.0, root.fpsLimit / nativeFps))
        }
        player.playbackRate = rate
    }

    // Emitted when crossfade transition completes
    signal transitionFinished

    property int _activePlayer: CrossFadeVideoPlayer.Player.One
    property bool _initialized: false
    property bool _transitioning: false

    onVolumeChanged: {
        // Only update active player's volume when NOT transitioning
        if (!_transitioning) {
            if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
                ao1.volume = volume
            } else {
                ao2.volume = volume
            }
        }
    }

    onNormalizedPositionChanged: {
        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            mediaPlayer1.position = normalizedPosition * mediaPlayer1.duration
        } else {
            mediaPlayer2.position = normalizedPosition * mediaPlayer2.duration
        }
    }

    onSourceChanged: {
        if (!_initialized) {
            ao1.volume = root.volume
            mediaPlayer1.source = root.source
            mediaPlayer1.play()
            root._initialized = true
            return
        }

        // Stop any ongoing transition
        fadeOut.stop()
        ao1FadeIn.stop()
        ao1FadeOut.stop()
        ao2FadeIn.stop()
        ao2FadeOut.stop()
        _transitioning = false

        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            // Switching TO player Two
            mediaPlayer2.source = root.source
            ao2.volume = 0;
            // Incoming starts silent

            vo2.z = 0
            vo2.opacity = 0
            vo1.z = 1
            vo1.opacity = 1

            fadeOut.target = vo1
            root._activePlayer = CrossFadeVideoPlayer.Player.Two
        } else {
            // Switching TO player One
            mediaPlayer1.source = source
            ao1.volume = 0;
            // Incoming starts silent

            vo1.z = 0
            vo1.opacity = 0
            vo2.z = 1
            vo2.opacity = 1

            fadeOut.target = vo2
            root._activePlayer = CrossFadeVideoPlayer.Player.One
        }

        startCrossFadeTimer.start()
    }

    onIsPlayingChanged: {
        if (_initialized) {
            if (isPlaying) {
                if (root._activePlayer === CrossFadeVideoPlayer.Player.One) {
                    mediaPlayer1.play()
                } else {
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
        audioOutput: ao1
        onMetaDataChanged: root._applyPlaybackRate(mediaPlayer1)
    }

    MediaPlayer {
        id: mediaPlayer2
        loops: root.loops ? MediaPlayer.Infinite : 1
        videoOutput: vo2
        audioOutput: ao2
        onMetaDataChanged: root._applyPlaybackRate(mediaPlayer2)
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

    AudioOutput {
        id: ao1
        muted: root.muted
    }

    AudioOutput {
        id: ao2
        volume: 0
        muted: root.muted
    }

    Timer {
        id: startCrossFadeTimer
        interval: 16
        repeat: true
        property int maxAttempts: 60
        property int attempts: 0

        onTriggered: {
            attempts++

            const incomingPlayer = root._activePlayer === CrossFadeVideoPlayer.Player.One ? mediaPlayer1 : mediaPlayer2
            const incomingOutput = root._activePlayer === CrossFadeVideoPlayer.Player.One ? vo1 : vo2

            if (incomingPlayer.hasVideo && incomingPlayer.playbackState === MediaPlayer.PlayingState) {
                stop()
                attempts = 0
                startCrossFade(incomingPlayer, incomingOutput)
            } else if (attempts >= maxAttempts) {
                console.warn("Video warmup timeout, forcing crossfade")
                stop()
                attempts = 0
                incomingPlayer.play()
                startCrossFade(incomingPlayer, incomingOutput)
            } else {
                if (incomingPlayer.playbackState !== MediaPlayer.PlayingState) {
                    incomingPlayer.play()
                }
            }
        }
    }

    function startCrossFade(incomingPlayer, incomingOutput) {
        _transitioning = true

        incomingPlayer.play()
        incomingOutput.opacity = 1
        fadeOut.start();

        // Audio crossfade: outgoing fades from current volume to 0
        //                  incoming fades from 0 to targetVolume
        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            ao1FadeIn.to = root.targetVolume
            ao1FadeIn.start()
            ao2FadeOut.start()
        } else {
            ao2FadeIn.to = root.targetVolume
            ao2FadeIn.start()
            ao1FadeOut.start()
        }
    }

    function finishTransition() {
        _transitioning = false
        root.transitionFinished()
    }

    // Video opacity fade
    NumberAnimation {
        id: fadeOut
        property: "opacity"
        from: 1
        to: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
        onFinished: {
            if (target === vo1) {
                mediaPlayer1.stop()
                vo1.z = 0
                vo2.z = 1
            } else {
                mediaPlayer2.stop()
                vo2.z = 0
                vo1.z = 1
            }
            finishTransition()
        }
    }

    // Audio fades
    NumberAnimation {
        id: ao1FadeIn
        target: ao1
        property: "volume"
        from: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: ao1FadeOut
        target: ao1
        property: "volume"
        to: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: ao2FadeIn
        target: ao2
        property: "volume"
        from: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: ao2FadeOut
        target: ao2
        property: "volume"
        to: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
    }
}
