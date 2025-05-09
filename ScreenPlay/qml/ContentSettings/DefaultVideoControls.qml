import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay
import ScreenPlayCore
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

FocusScope {
    id: root
    state: "hidden"

    property bool timelineActive
    property int monitorIndex
    property int timelineIndex
    property string sectionIdentifier
    property var wallpaperData
    property bool hasContent: false
    onWallpaperDataChanged: {
        if (!wallpaperData)
            return;
        root.hasContent = wallpaperData.hasContent();
        slVolume.slider.value = wallpaperData.volume.toFixed(2);
        settingsComboBox.currentIndex = settingsComboBox.indexOfValue(wallpaperData.fillMode);
    }

    Text {
        anchors.fill: parent
        anchors.margins: 20
        visible: !root.hasContent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: Material.secondaryTextColor
        text: qsTr("No timeline section selected")
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        clip: true
        visible: root.hasContent

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
            iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_volume.svg"
            slider.stepSize: 0.1
            Layout.fillWidth: true
            Layout.topMargin: 20
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            onValueEditingFinished: {

                // settingValue = true;
                const newVolume = slVolume.slider.value.toFixed(2);
                console.log(root.timelineActive, root.monitorIndex, root.timelineIndex, root.sectionIdentifier, newVolume);
                const category = "";
                App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "volume", newVolume, category).then(result => {
                    settingValue = false;
                    if (!result.success) {
                        InstantPopup.openErrorPopup(root, result.message);
                    }
                });
            }
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
                    App.screenPlayManager.setWallpaperFillModeAtMonitorIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, settingsComboBox.currentValue).then(result => {
                        settingValue = false;
                        if (!result.success) {
                            InstantPopup.openErrorPopup(root, result.message);
                        }
                    });
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
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
