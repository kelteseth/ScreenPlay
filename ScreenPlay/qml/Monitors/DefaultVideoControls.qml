import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import ScreenPlay 1.0
import ScreenPlay.Enums.FillMode 1.0


import "../Common/" as SP

ColumnLayout {
    id: root
    spacing: 20
    state: "hidden"
    clip: true

    function indexOfValue(model, value) {

        for (var i = 0; i < model.length; i++) {
            let ourValue = model[i].value
            if (value === ourValue)
                return i
        }
        return -1
    }

    property int activeMonitorIndex

    SP.Slider {
        headline: qsTr("Volume")
        onValueChanged: ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                            activeMonitorIndex, "volume", value)
        Layout.fillWidth: true
    }
    SP.Slider {
        headline: qsTr("Playback rate")
        onValueChanged: ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                            activeMonitorIndex, "playbackRate", value)
        Layout.fillWidth: true
    }
    SP.Slider {
        headline: qsTr("Current Video Time")
        onValueChanged: ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                            activeMonitorIndex, "currentTime", value)
        Layout.fillWidth: true
    }
    ColumnLayout {
        height: 50
        Layout.fillWidth: true
        Layout.margins: 10
        spacing: 5

        Text {
            id: txtComboBoxFillMode
            text: qsTr("Fill Mode")
            font.family: ScreenPlay.settings.font
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 10
            color: "#626262"
            wrapMode: Text.WrapAnywhere
            Layout.fillWidth: true
        }
        ComboBox {
            id: settingsComboBox
            Layout.fillWidth: true
            Layout.leftMargin: 10
            onActivated: {
                ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                            activeMonitorIndex, "fillmode",
                            settingsComboBox.currentText)
            }

            textRole: "text"
            valueRole: "value"
            currentIndex:   root.indexOfValue(
                            settingsComboBox.model,
                            ScreenPlay.settings.videoFillMode)


            model: [{
                    "value": FillMode.Stretch,
                    "text": qsTr("Stretch")
                }, {
                    "value": FillMode.Fill,
                    "text": qsTr("Fill")
                }, {
                    "value": FillMode.Contain,
                    "text": qsTr("Contain")
                }, {
                    "value": FillMode.Cover,
                    "text": qsTr("Cover")
                }, {
                    "value": FillMode.Scale_Down,
                    "text": qsTr("Scale-Down")
                }]
        }
    }

    states: [
        State {
            name: "visible"
            PropertyChanges {
                target: root
                opacity: 1
                anchors.topMargin: 20
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: root
                opacity: 0
                anchors.topMargin: -50
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"
            reversible: true
            PropertyAnimation {
                target: root
                duration: 300
                easing.type: Easing.InOutQuart
                properties: "anchors.topMargin, opacity"
            }
        }
    ]
}
