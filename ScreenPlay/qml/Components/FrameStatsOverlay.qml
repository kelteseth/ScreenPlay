// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import QtQuick
import ScreenPlay

/*!
    Frame pacing overlay (toggled with Ctrl+Shift+F in ScreenPlayMain).

    Shows presentation timing statistics from App.frameStats and a
    constant-velocity pacer bar: at a perfectly paced refresh rate the bar
    moves by the exact same amount every frame - any visible jump or the
    "step σ" value rising above ~a tenth of a pixel means displayed motion
    is uneven, which is what "feels like 30 fps" usually is.
*/
Rectangle {
    id: root
    objectName: "frameStatsOverlay"

    property bool shown: false

    visible: shown
    onShownChanged: {
        if (shown) {
            App.frameStats.attach(root.Window.window)
        }
        App.frameStats.enabled = shown
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
            text: App.frameStats.fps.toFixed(1) + " fps"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "target   " + (1000 / App.frameStats.expectedMs).toFixed(0) + " Hz (" + App.frameStats.expectedMs.toFixed(2) + " ms)"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "mean     " + App.frameStats.meanMs.toFixed(2) + " ms   σ " + App.frameStats.stddevMs.toFixed(2) + " ms"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "p99      " + App.frameStats.p99Ms.toFixed(2) + " ms   worst " + App.frameStats.worstMs.toFixed(2) + " ms"
        }
        Text {
            color: App.frameStats.stutterCount > 0 ? "#FFB74D" : "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "stutters " + App.frameStats.stutterCount + " (>1.5x frame)"
        }
        Text {
            color: "#BBBBBB"
            font.family: "Roboto Mono"
            font.pixelSize: 12
            text: "step σ   " + App.frameStats.animStepStddevPx.toFixed(3) + " px   api " + App.frameStats.apiName
        }
        Text {
            color: "#777777"
            font.family: "Roboto Mono"
            font.pixelSize: 10
            text: "env " + App.frameStats.envSummary
        }

        // Constant-velocity pacer: drives continuous rendering while the
        // overlay is open and feeds the animation-step metric.
        Rectangle {
            width: 220
            height: 8
            radius: 2
            color: "#22FFFFFF"

            Rectangle {
                id: pacer
                width: 30
                height: parent.height
                radius: 2
                color: "#FF9800"

                NumberAnimation on x {
                    running: root.shown
                    from: 0
                    to: 190
                    duration: 1000
                    loops: Animation.Infinite
                }
            }
        }
    }

    Connections {
        target: root.Window.window
        enabled: root.shown

        function onAfterAnimating() {
            App.frameStats.recordAnimSample(pacer.x)
        }
    }
}
