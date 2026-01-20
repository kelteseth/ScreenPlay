import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Controls.Material.impl
import ScreenPlayWorkshop
import ScreenPlayCore

Page {
    id: root

    property string previewImagePath
    property real progress: 0
    property string name: "Headline"
    property var steamStatus
    property int uploadState: 0 // EItemUpdateStatus enum

    // Calculate effective progress: use byte progress if available, otherwise estimate from state
    readonly property real effectiveProgress: {
        if (root.progress > 0) {
            return root.progress
        }
        // Estimate progress based on upload state when bytes not available
        switch (root.uploadState) {
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusInvalid:
            return 0
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusPreparingConfig:
            return 10
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusPreparingContent:
            return 20
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusUploadingContent:
            return 40
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusUploadingPreviewFile:
            return 70
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusCommittingChanges:
            return 90
        default:
            return 0
        }
    }

    // Map uploadState to human-readable status text
    function uploadStateText(state: int): string {
        switch (state) {
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusInvalid:
            return qsTr("Waiting...")
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusPreparingConfig:
            return qsTr("Preparing Config...")
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusPreparingContent:
            return qsTr("Preparing Content...")
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusUploadingContent:
            return qsTr("Uploading Content...")
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusUploadingPreviewFile:
            return qsTr("Uploading Preview...")
        case Steam.EItemUpdateStatus.K_EItemUpdateStatusCommittingChanges:
            return qsTr("Committing Changes...")
        default:
            return qsTr("Processing...")
        }
    }

    width: 800
    height: 240
    padding: 20
    onPreviewImagePathChanged: img.source = Qt.resolvedUrl("file:///" + previewImagePath)
    // Everything that is not OK is a fail. See steam_qt_enums_generated.h
    // EResult: 0=None, 1=OK, 2=Fail, etc.
    onSteamStatusChanged: {
        // Handle success cases first (K_EResultNone=0, K_EResultOK=1)
        if (steamStatus === Steam.EResult.K_EResultNone || steamStatus === Steam.EResult.K_EResultOK) {
            root.contentItem.state = "uploadComplete"
            return
        }

        // For errors, show a generic message with the error code
        // This avoids the massive switch statement while still being informative
        root.contentItem.txtStatus.statusText = qsTr("Error: %1").arg(steamStatus)
        root.contentItem.state = "error"
    }

    background: Rectangle {
        radius: 4
        anchors.fill: parent
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 6
        }
    }

    contentItem: Item {
        anchors.fill: parent
        state: "base"

        Image {
            id: img

            // 16:9 aspect ratio, width calculated from parent height
            width: parent.height * (16 / 9)

            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
            }
        }

        ColumnLayout {
            spacing: 10

            anchors {
                top: parent.top
                right: parent.right
                left: img.right
                margins: 20
            }

            Text {
                id: name

                text: root.name
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: Material.primaryTextColor
                font.pointSize: 18
                Layout.preferredHeight: 30
                Layout.fillWidth: true
            }

            Text {
                id: txtStatus

                property string statusText: root.uploadStateText(root.uploadState)

                text: qsTr("Status:") + " " + statusText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: Material.primaryTextColor
                font.pointSize: 14
                Layout.preferredHeight: 30
            }

            Item {
                Layout.preferredHeight: 60
                Layout.fillWidth: true
            }

            ColumnLayout {
                spacing: 10
                Layout.fillWidth: true

                Text {
                    text: qsTr("Progress:") + " " + Math.ceil(root.effectiveProgress) + "%"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: Material.primaryTextColor
                    font.pointSize: 14
                    height: 50
                }

                ProgressBar {
                    id: progressBar

                    Layout.fillWidth: true
                    value: root.effectiveProgress
                    to: 100
                }
            }
        }

        states: [
            State {
                name: "uploading"
            },
            State {
                name: "uploadComplete"
            },
            State {
                name: "error"
            }
        ]
    }
}
