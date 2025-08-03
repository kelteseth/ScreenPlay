// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material

Item {
    id: root
    implicitWidth: 240
    property int totalMinutes: 60
    property int remainingMinutes: Math.max(0, 60 - new Date().getMinutes())
    property int totalHours: 24
    property int remainingHours: Math.max(0, Math.floor((new Date().setHours(24, 0, 0, 0) - new Date()) / 3600000))
    property int remainingDays: Math.max(0, Math.floor((new Date(new Date().getFullYear() + 1, 0, 1) - new Date()) / (24 * 60 * 60 * 1000)))

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        ColumnLayout {
            Layout.fillWidth: true
            Label {
                id: countdownTextMinutes
                text: remainingMinutes + qsTr(" minutes left this hour")
                Layout.fillWidth: true
            }

            ProgressBar {
                id: countdownProgressMinutes
                Layout.fillWidth: true
                height: 10
                value: totalMinutes - remainingMinutes
                to: totalMinutes
                Timer {
                    interval: 1000
                    running: true
                    repeat: true
                    onTriggered: {
                        remainingMinutes = Math.max(0, 60 - new Date().getMinutes());
                    }
                }
            }
        }
        ColumnLayout {

            Layout.fillWidth: true
            Label {
                id: countdownTextHours
                text: remainingHours + qsTr(" hours left today")
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

            Label {
                id: countdownTextDays
                text: remainingDays + qsTr(" days left this year")
                Layout.fillWidth: true
            }

            ProgressBar {
                id: countdownProgressDays
                Layout.fillWidth: true
                height: 10
                value: 365 - remainingDays
                to: 365

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
