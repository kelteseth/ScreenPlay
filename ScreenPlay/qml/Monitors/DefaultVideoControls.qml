import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Enums.FillMode
import ScreenPlayUtil as Util

ColumnLayout {
    id: root

    property int activeMonitorIndex
    property ScreenPlayWallpaper wallpaper

    function indexOfValue(model, value) {
        for (var i = 0; i < model.length; i++) {
            let ourValue = model[i].value;
            if (value === ourValue)
                return i;
        }
        return -1;
    }

    spacing: 10
    state: "hidden"
    clip: true
    onWallpaperChanged: {
        slVolume.slider.value = wallpaper.volume;
    }

    Util.Slider {
        id: slVolume

        headline: qsTr("Volume")
        slider.stepSize: 0.1
        Layout.fillWidth: true
        Layout.leftMargin: 10
        Layout.rightMargin: 10
        slider.onValueChanged: {
            App.screenPlayManager.setWallpaperValueAtMonitorIndex(activeMonitorIndex, "volume", (Math.round(slVolume.slider.value * 100) / 100));
        }
    }

    Util.Slider {
        id: slCurrentVideoTime

        headline: qsTr("Current Video Time")
        slider.onValueChanged: App.screenPlayManager.setWallpaperValueAtMonitorIndex(activeMonitorIndex, "currentTime", (Math.round(slCurrentVideoTime.slider.value * 100) / 100))
        Layout.fillWidth: true
        slider.stepSize: 0.1
        Layout.leftMargin: 10
        Layout.rightMargin: 10
    }

    ColumnLayout {
        height: 50
        Layout.fillWidth: true
        Layout.margins: 10
        spacing: 5

        Text {
            id: txtComboBoxFillMode

            text: qsTr("Fill Mode")
            font.family: App.settings.font
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
            textRole: "text"
            valueRole: "value"
            currentIndex: root.indexOfValue(settingsComboBox.model, App.settings.videoFillMode)
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
                    "text": qsTr("Scale_Down")
                }]
            onActivated: {
                App.screenPlayManager.setWallpaperFillModeAtMonitorIndex(activeMonitorIndex, settingsComboBox.currentValue);
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
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
