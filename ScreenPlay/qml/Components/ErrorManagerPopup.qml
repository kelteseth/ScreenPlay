pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls.Material.impl
import ScreenPlay
import ScreenPlayCore as Util
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

Dialog {
    id: root

    property alias errorModel: listView.model
    property Item modalSource

    title: qsTr("Errors")
    modal: true
    closePolicy: Dialog.CloseOnEscape | Dialog.CloseOnPressOutside

    anchors.centerIn: root.modalSource
    width: Math.min(600, parent.width - 40)
    height: Math.min(400, parent.height - 40)

    onAboutToHide: {
        // Fixes the ugly transition with
        // our ModalBackgroundBlur on exit
        modal = false
    }

    onAboutToShow: {
        modal = true
    }

    Component.onCompleted: {
        // When we have errors before qml is loaded:
        if (App.errorManager.hasErrors) {
            openDelayTimer.start()
        }
    }
    Timer {
        // For the installed content to fade in first.
        id: openDelayTimer
        interval: 1000
        onTriggered: {
            root.open()
        }
    }

    // Auto-open when there are new errors (only if not already open)
    Connections {
        target: App.errorManager

        function onShowErrorPopup() {
            if (!root.opened) {
                root.open()
            }
        }

        ignoreUnknownSignals: true
    }

    Overlay.modal: Util.ModalBackgroundBlur {
        id: blurBg
        sourceItem: root.modalSource
        colorOverlayOpacity: .4
        blur: .5
    }
    background: Rectangle {
        anchors.fill: parent
        radius: 4
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 6
        }
    }

    Material.accent: Material.Red

    header: Rectangle {
        height: 80
        color: Material.dialogColor
        topLeftRadius: 4
        topRightRadius: 4

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20

            Text {
                text: qsTr("⚠️ Something unexpected happened")
                color: Material.primaryTextColor
                font.pointSize: 16
                font.weight: Font.Medium
                Layout.fillWidth: true
            }
            Text {
                id: errorCountText
                Layout.preferredWidth: Math.max(
                                           24, errorCountText.implicitWidth + 8)
                Layout.preferredHeight: 24
                text: App.errorManager?.errorCount ?? 0
                color: Material.color(Material.Red)
                font.pointSize: 14
                font.weight: Font.DemiBold
            }
            ToolButton {
                icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_close.svg"
                icon.color: Material.iconColor
                icon.width: 20
                icon.height: 20
                onClicked: root.close()
            }
        }
    }
    contentItem: ColumnLayout {
        spacing: 0

        ScrollView {
            visible: listView.count > 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 16

            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            ListView {
                id: listView

                model: App.errorManager
                spacing: 12
                delegate: Rectangle {
                    id: delegate
                    required property string message
                    required property string formattedTimestamp

                    width: ListView.view.width
                    height: errorLayout.implicitHeight + 24

                    color: Material.theme
                           === Material.Dark ? Material.color(
                                                   Material.Grey,
                                                   Material.Shade800) : Material.color(
                                                   Material.Grey,
                                                   Material.Shade50)
                    // radius: 12
                    // clip: true
                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 4
                        color: Material.color(Material.Red)
                    }

                    ColumnLayout {
                        id: errorLayout
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 12
                        anchors.topMargin: 12
                        anchors.bottomMargin: 12
                        spacing: 6

                        Text {
                            text: delegate.formattedTimestamp
                            color: Material.hintTextColor
                            font.pointSize: 10
                            Layout.fillWidth: true
                        }
                        Text {
                            text: delegate.message
                            color: Material.primaryTextColor
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                            font.pointSize: 12
                        }
                    }
                }

                // Auto-scroll to bottom when new errors are added
                onCountChanged: {
                    if (count > 0) {
                        positionViewAtEnd()
                    }
                }
            }
        }

        // Empty state
        Item {
            visible: listView.count === 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 32

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 16
                Rectangle {
                    Layout.alignment: Qt.AlignCenter
                    width: 48
                    height: 48
                    radius: 24
                    color: Material.theme
                           === Material.Dark ? Material.color(
                                                   Material.Grey,
                                                   Material.Shade700) : Material.color(
                                                   Material.Grey,
                                                   Material.Shade200)

                    Text {
                        anchors.centerIn: parent
                        text: "✓"
                        color: Material.color(Material.Green)
                        font.pointSize: 20
                        font.weight: Font.Bold
                    }
                }

                Text {
                    Layout.alignment: Qt.AlignCenter
                    text: qsTr("No errors to display")
                    color: Material.hintTextColor
                    font.pointSize: 14
                }
            }
        }
    }
    footer: Rectangle {
        height: 60
        color: Material.dialogColor
        bottomRightRadius: 4
        bottomLeftRadius: 4

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: Material.color(Material.Grey, Material.Shade300)
            opacity: 0.5
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            Button {
                text: qsTr("Clear All")
                enabled: App.errorManager?.hasErrors ?? false
                onClicked: App.errorManager?.clearErrors()
                flat: true
                Material.foreground: Material.color(Material.Orange)
            }

            Item {
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("Ask about it in the forums")
                onClicked: Qt.openUrlExternally("https://forum.screen-play.app/");
                Material.background: Material.primary
                Material.foreground: Material.primaryTextColor
            }

            Button {
                text: qsTr("Close")
                onClicked: root.close()
                Material.background: Material.primary
                Material.foreground: Material.primaryTextColor
            }
        }
    }
}
