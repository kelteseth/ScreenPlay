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
    property bool isPlaying: false
    property real normalizedPosition: 0

    property int _activePlayer: CrossFadeVideoPlayer.Player.One
    property bool _initialized: false

    onNormalizedPositionChanged: {
        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            mediaPlayer1.position = normalizedPosition * mediaPlayer1.duration;
        } else {
            mediaPlayer2.position = normalizedPosition * mediaPlayer2.duration;
        }
    }

    onSourceChanged: {
        if (!_initialized) {
            mediaPlayer1.source = root.source;
            mediaPlayer1.play();
            root._initialized = true;
            root.isPlaying = true;
            return;
        }

        // Stop any ongoing animations
        fadeIn.stop();
        fadeOut.stop();

        if (_activePlayer === CrossFadeVideoPlayer.Player.One) {
            // Prepare player two for transition
            mediaPlayer2.source = root.source;
            mediaPlayer2.play();

            // Set initial states
            vo1.opacity = 1;
            vo2.opacity = 0;

            // Configure crossfade for player two
            fadeIn.target = vo2;
            fadeOut.target = vo1;

            root._activePlayer = CrossFadeVideoPlayer.Player.Two;
        } else {
            // Prepare player one for transition
            mediaPlayer1.source = source;
            mediaPlayer1.play();

            // Set initial states
            vo1.opacity = 0;
            vo2.opacity = 1;

            // Configure crossfade for player one
            fadeIn.target = vo1;
            fadeOut.target = vo2;

            root._activePlayer = CrossFadeVideoPlayer.Player.One;
        }

        // Start the transition after a small delay
        startCrossFadeTimer.start();
    }

    onIsPlayingChanged: {
        if (isPlaying) {
            root._activePlayer === CrossFadeVideoPlayer.Player.One ? mediaPlayer1.play() : mediaPlayer2.play();
        } else {
            mediaPlayer1.pause();
            mediaPlayer2.pause();
        }
    }

    onFillModeChanged: {
        let qtFillMode;
        if (fillMode === "cover" || fillMode === "stretch" || fillMode === "contain") {
            qtFillMode = VideoOutput.Stretch;
        } else if (fillMode === "fill") {
            qtFillMode = VideoOutput.PreserveAspectFit;
        } else if (fillMode === "scale_down") {
            qtFillMode = VideoOutput.PreserveAspectCrop;
        }
        vo1.fillMode = qtFillMode;
        vo2.fillMode = qtFillMode;
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
    }

    VideoOutput {
        id: vo2
        anchors.fill: parent
        opacity: 0
    }

    Timer {
        id: startCrossFadeTimer
        interval: 500  // Small delay to ensure video has started
        onTriggered: {
            fadeIn.start();
            fadeOut.start();
        }
    }

    // Split the crossfade into two parallel animations
    NumberAnimation {
        id: fadeIn
        property: "opacity"
        from: 0
        to: 1
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: fadeOut
        property: "opacity"
        from: 1
        to: 0
        duration: root.crossFadeDuration
        easing.type: Easing.InOutQuad
        onFinished: {
            // Stop the previous player only after fade out is complete
            if (target === vo1) {
                mediaPlayer1.stop();
            } else {
                mediaPlayer2.stop();
            }
        }
    }

    AudioOutput {
        id: ao
        volume: root.volume
        muted: root.muted
    }
}
