// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import QtQuick

/*!
    Frame pacing overlay. Reusable from any window (main app, wallpaper):

        FrameStatsOverlay {
            stats: App.frameStats  // or a locally created FrameStats {}
            shown: true
        }

    Shows presentation timing statistics, a per-frame spike graph (last 160
    frame deltas, red above 1.5x the expected interval, cyan line at the
    expected interval) and a constant-velocity pacer bar: at a perfectly
    paced refresh rate the bar moves by the exact same amount every frame -
    any visible jump or a rising "step σ" means displayed motion is uneven.
*/
Rectangle {
    id: root
    objectName: "frameStatsOverlay"

    required property FrameStats stats
    property bool shown: false

    // Invisible constant-velocity value feeding the animation-step metric
    // ("what the eye sees"): every displayed frame should advance it by the
    // exact same amount. FrameStats drives continuous rendering itself, so
    // no visible animation is needed.
    property real pacerX: 0
    NumberAnimation on pacerX {
        running: root.shown
        from: 0
        to: 230
        duration: 1000
        loops: Animation.Infinite
    }

    visible: shown
    onShownChanged: {
        if (shown) {
            root.stats.attach(root.Window.window)
        }
        root.stats.enabled = shown
    }

    width: statsColumn.width + 24
    height: statsColumn.height + 24
    radius: 4
    color: "#DD101010"
    border.color: "#33FFFFFF"
    border.width: 1

    Column {
        id: statsColumn
        x: 12
        y: 12
        spacing: 2

        Text {
            color: "white"
            font.pixelSize: 24
            font.bold: true
            text: root.stats.fps.toFixed(1) + " fps"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "target   " + (1000 / root.stats.expectedMs).toFixed(0) + " Hz (" + root.stats.expectedMs.toFixed(2) + " ms)"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "mean     " + root.stats.meanMs.toFixed(2) + " ms   σ " + root.stats.stddevMs.toFixed(2) + " ms"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "p99      " + root.stats.p99Ms.toFixed(2) + " ms   worst " + root.stats.worstMs.toFixed(2) + " ms"
        }
        Text {
            color: root.stats.stutterCount > 0 ? "#FFB74D" : "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "stutters " + root.stats.stutterCount + " (>1.5x, last 5s)"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "step σ   " + root.stats.animStepStddevPx.toFixed(3) + " px   api " + root.stats.apiName
        }

        // Per-frame spike graph: one bar per presented frame, refreshed at
        // 10 Hz to stay lightweight. Height scale caps at 4x the expected
        // frame interval.
        Canvas {
            id: graph
            width: 260
            height: 90

            property var deltas: []

            Timer {
                interval: 100
                running: root.shown
                repeat: true
                onTriggered: {
                    graph.deltas = root.stats.recentDeltas(160)
                    graph.requestPaint()
                }
            }

            onPaint: {
                const ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                const exp = root.stats.expectedMs
                const scaleY = height / (exp * 4)

                // reference line at the expected frame interval
                ctx.strokeStyle = "#4DD0E1"
                ctx.beginPath()
                ctx.moveTo(0, height - exp * scaleY)
                ctx.lineTo(width, height - exp * scaleY)
                ctx.stroke()

                const barW = width / 160
                for (let i = 0; i < graph.deltas.length; ++i) {
                    const d = graph.deltas[i]
                    ctx.fillStyle = d > exp * 1.5 ? "#EF5350" : "#9E9E9E"
                    const h = Math.min(height, d * scaleY)
                    ctx.fillRect(i * barW, height - h, Math.max(1, barW - 1), h)
                }
            }
        }

        Text {
            width: 260
            color: "#777777"
            font.family: "Roboto Mono"
            font.pixelSize: 10
            wrapMode: Text.Wrap
            text: "env " + root.stats.envSummary
        }
    }

    Connections {
        target: root.Window.window
        enabled: root.shown

        function onAfterAnimating() {
            root.stats.recordAnimSample(root.pacerX)
        }
    }
}
