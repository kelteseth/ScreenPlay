import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlayCore

ColumnLayout {
    id: root

    property bool timelineActive
    property int monitorIndex
    property int timelineIndex
    property string sectionIdentifier
    property var wallpaperData
    onWallpaperDataChanged: {
        if (!wallpaperData)
            return;
        slVolume.slider.value = wallpaperData.volume.toFixed(2);
        settingsComboBox.currentIndex = settingsComboBox.indexOfValue(wallpaperData.fillMode);
    }

    spacing: 10
    state: "hidden"
    clip: true

    Label {
        Layout.fillWidth: true
        Layout.preferredHeight: 30
        Layout.leftMargin: 10
        font.pointSize: 14
        wrapMode: Text.WrapAnywhere
        elide: Text.ElideRight
        color: root.timelineActive ? Material.primaryTextColor : Material.secondaryTextColor
        text: root.wallpaperData ? root.wallpaperData.title : ""
    }

    LabelSlider {
        id: slVolume

        headline: qsTr("Volume")
        iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/icon_volume.svg"
        slider.stepSize: 0.1
        Layout.fillWidth: true
        Layout.topMargin: 20
        Layout.leftMargin: 10
        Layout.rightMargin: 10
        property bool settingValue: false
        onValueEditingFinished: {
            console.log(root.timelineActive, root.monitorIndex, root.timelineIndex, root.sectionIdentifier);
            if (settingValue) {
                console.error("Setting settingValue to fast");
                return;
            }
            settingValue = true;
            App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "volume", slVolume.slider.value.toFixed(2)).then(result => {
                settingValue = false;
                if (!result.success) {
                    console.error("setValueAtMonitorTimelineIndex failed");
                    return;
                }
                console.debug("OK setValueAtMonitorTimelineIndex ");
            });
        }
    }

    LabelSlider {
        id: slCurrentVideoTime
        iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/icon_schedule.svg"

        headline: qsTr("Current Video Time")
        slider.onValueChanged: {
            App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "currentTime", slCurrentVideoTime.slider.value.toFixed(2));
        }
        Layout.fillWidth: true
        slider.stepSize: 0.1
        Layout.leftMargin: 10
        Layout.rightMargin: 10
    }

    ColumnLayout {
        implicitHeight: 50
        spacing: 15
        Layout.fillWidth: true
        Layout.leftMargin: 10
        Layout.rightMargin: 10

        Text {
            id: txtComboBoxFillMode
            height: 20
            font.pointSize: 14

            text: qsTr("Fill Mode")
            verticalAlignment: Text.AlignVCenter
            font.family: App.settings.font
            color: Material.primaryTextColor
            wrapMode: Text.WrapAnywhere
            Layout.fillWidth: true
        }

        ComboBox {
            id: settingsComboBox
            Layout.fillWidth: true
            Layout.leftMargin: 10
            textRole: "text"
            valueRole: "value"

            model: ListModel {
                id: model
                ListElement {
                    value: Video.FillMode.Stretch
                    text: qsTr("Stretch")
                }

                ListElement {
                    value: Video.FillMode.Fill
                    text: qsTr("Fill")
                }

                ListElement {
                    value: Video.FillMode.Contain
                    text: qsTr("Contain")
                }
                ListElement {
                    value: Video.FillMode.Cover
                    text: qsTr("Cover")
                }
                ListElement {
                    value: Video.FillMode.Scale_Down
                    text: qsTr("Scale Down")
                }
            }
            onActivated: {
                App.screenPlayManager.setWallpaperFillModeAtMonitorIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, settingsComboBox.currentValue);
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
