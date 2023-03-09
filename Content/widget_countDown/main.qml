// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Particles

Item {
    id: root
    implicitWidth: 240
    implicitHeight: 120
    property int totalHours: 24
    property int remainingHours: Math.max(0, Math.floor((new Date().setHours(24, 0, 0, 0) - new Date()) / 3600000))
    property int totalDays: new Date(new Date().getFullYear() + 1, 0, 1) - new Date() / (24 * 60 * 60 * 1000)
    property int remainingDays: Math.max(0, Math.floor((new Date(new Date().getFullYear() + 1, 0, 1) - new Date()) / (24 * 60 * 60 * 1000)))

    Material.theme: Material.Dark
    Material.accent: Material.DeepOrange

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        ColumnLayout {

            Layout.fillWidth: true
            Text {
                id: countdownTextHours
                color: Material.primaryTextColor
                text: remainingHours + " hours left today"
                Layout.fillWidth: true
            }

            ProgressBar {
                id: countdownProgressHours
                Layout.fillWidth: true
                height: 10
                value: totalHours - remainingHours
                to: totalHours
                Timer {
                    interval: 1000
                    running: true
                    repeat: true
                    onTriggered: {
                        remainingHours = Math.max(0, Math.floor((new Date().setHours(24, 0, 0, 0) - new Date()) / 3600000));
                    }
                }
            }
        }
        ColumnLayout {
            Layout.fillWidth: true

            Text {
                id: countdownTextDays
                color: Material.primaryTextColor
                text: remainingDays + " days left this year"
                Layout.fillWidth: true
            }

            ProgressBar {
                id: countdownProgressDays
                Layout.fillWidth: true
                height: 10
                value: totalDays - remainingDays
                to: totalDays

                Timer {
                    interval: 1000 * 60
                    running: true
                    repeat: true
                    onTriggered: {
                        remainingDays = Math.max(0, Math.floor((new Date(new Date().getFullYear() + 1, 0, 1) - new Date()) / (24 * 60 * 60 * 1000)));
                    }
                }
            }
        }
    }
}
