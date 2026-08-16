// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay
import ScreenPlayCore
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

// Per-wallpaper render fps cap for non-Godot wallpapers. The stored value -1
// means "inherit the global setting", so an untouched wallpaper shows the
// global value as its selection; picking any entry stores an explicit override
// in the profile and applies it live.
ColumnLayout {
    id: root

    property int monitorIndex
    property int timelineIndex
    property string sectionIdentifier
    property var wallpaperData

    spacing: 6

    function reload(): void {
        if (!wallpaperData)
            return
        const fps = wallpaperData.fpsLimit >= 0 ? wallpaperData.fpsLimit : App.settings.wallpaperFpsLimit
        cbFpsLimit.currentIndex = cbFpsLimit.indexOfValue(fps)
    }

    onWallpaperDataChanged: root.reload()

    Text {
        height: 20
        font.pointSize: 14
        text: qsTr("FPS limit")
        verticalAlignment: Text.AlignVCenter
        font.family: App.settings.font
        color: Material.primaryTextColor
        wrapMode: Text.WrapAnywhere
        Layout.fillWidth: true
    }

    ComboBox {
        id: cbFpsLimit
        Layout.fillWidth: true
        textRole: "text"
        valueRole: "value"

        model: ListModel {
            ListElement {
                value: 0
                text: qsTr("Unlimited")
            }
            ListElement {
                value: 1
                text: qsTr("1 FPS")
            }
            ListElement {
                value: 6
                text: qsTr("6 FPS")
            }
            ListElement {
                value: 12
                text: qsTr("12 FPS")
            }
            ListElement {
                value: 24
                text: qsTr("24 FPS")
            }
            ListElement {
                value: 30
                text: qsTr("30 FPS")
            }
            ListElement {
                value: 60
                text: qsTr("60 FPS")
            }
            ListElement {
                value: 90
                text: qsTr("90 FPS")
            }
            ListElement {
                value: 120
                text: qsTr("120 FPS")
            }
            ListElement {
                value: 144
                text: qsTr("144 FPS")
            }
        }
        onActivated: {
            App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "fpsLimit", cbFpsLimit.currentValue, "").then(result => {
                if (!result.success) {
                    InstantPopup.openErrorPopup(root, result.message)
                }
            })
        }
    }
}
