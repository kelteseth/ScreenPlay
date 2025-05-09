import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects
import ScreenPlay
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

Item {
    id: root

    required property string category
    required property string name
    required property bool isHeadline
    required property var value
    required property int index

    property int selectedMonitorIndex
    property bool timelineActive
    property int timelineIndex
    property string sectionIdentifier
    property var projectSettingsListmodelRef

    focus: true
    height: isHeadline ? 50 : 30

    Text {
        id: txtDescription

        text: root.name
        width: 100
        font.pointSize: root.isHeadline ? 18 : 12
        anchors.verticalCenter: parent.verticalCenter
        font.family: App.settings.font
        font.weight: Font.Normal
        color: root.isHeadline ? Qt.darker(Material.foreground) : Material.foreground

        anchors {
            left: parent.left
            leftMargin: root.isHeadline ? 0 : 25
        }
    }

    Item {
        height: parent.height
        visible: !root.isHeadline
        Component.onCompleted: {
            if (root.isHeadline)
                return;
            console.log("MonitorsProjectSettingItem:", root.value["type"]);
            switch (root.value["type"]) {
            case "slider":
                loader.sourceComponent = compSlider;
                loader.item.from = root.value["from"];
                loader.item.to = root.value["to"];
                loader.item.value = root.value["value"];
                loader.item.stepSize = root.value["stepSize"];
                const value = parseFloat(loader.item.value.toFixed(2));
                loader.item.text = value;
                break;
            case "bool":
                loader.sourceComponent = compCheckbox;
                loader.item.value = root.value["value"];
                break;
            case "color":
                loader.sourceComponent = compColorpicker;
                loader.item.value = root.value["value"];
                break;
            default:
                console.error(name, " has an invalid type:" << root.value["type"]);
                break;
            }
            if (root.value["text"])
                txtDescription.text = root.value["text"];
        }

        anchors {
            left: txtDescription.right
            leftMargin: 20
            right: parent.right
        }

        Loader {
            id: loader

            anchors.fill: parent
            anchors.rightMargin: 10

            Connections {
                function onSave(obj) {
                    print("on save value:", root.index, root.name, root.category, obj.value);
                    root.projectSettingsListmodelRef.setValueAtIndex(root.index, root.name, root.category, obj.value);

                    const monitorIndex = root.selectedMonitorIndex;
                    const timelineIndex = root.timelineIndex;
                    const sectionIdentifier = root.sectionIdentifier;
                    const key = root.name;
                    const value = obj.value;

                    App.screenPlayManager.setValueAtMonitorTimelineIndex(monitorIndex, timelineIndex, sectionIdentifier, key, value, root.category).then(result => {
                        settingValue = false;
                        if (!result.success) {
                            InstantPopup.openErrorPopup(root, result.message);
                        }
                    });
                }

                target: loader.item
            }
        }

        Component {
            id: compCheckbox

            Item {
                id: root

                property bool value

                signal save(var value)

                anchors.fill: parent

                CheckBox {
                    id: checkbox

                    checkable: true
                    checked: root.value
                    onPressed: {
                        let obj = {
                            "value": checkbox.checked,
                            "type": "checkBox"
                        };
                        root.save(obj);
                    }

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Component {
            id: compColorpicker

            Item {
                id: root

                property color value

                signal save(var value)

                anchors.fill: parent

                Button {
                    id: btnSetColor

                    text: qsTr("Set color")
                    onClicked: colorDialog.open()

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }

                Rectangle {
                    id: rctPreviewColor

                    radius: 3
                    color: root.value
                    border.width: 1
                    border.color: "gray"
                    width: parent.height
                    height: parent.height

                    anchors {
                        right: btnSetColor.left
                        rightMargin: 20
                        verticalCenter: parent.verticalCenter
                    }
                }

                ColorDialog {
                    id: colorDialog

                    title: qsTr("Please choose a color")
                    onAccepted: {
                        rctPreviewColor.color = colorDialog.selectedColor;
                        let obj = {
                            "value": rctPreviewColor.color,
                            "type": "color"
                        };
                        root.save(obj);
                    }
                }
            }
        }

        Component {
            id: compSlider

            Item {
                id: root

                property int from
                property string text
                property int to
                property int value
                property int stepSize: 1

                signal save(var value)

                anchors.fill: parent

                Slider {
                    id: slider

                    from: root.from
                    to: root.to
                    stepSize: root.stepSize
                    live: false
                    value: root.value
                    onVisualPositionChanged: root.text = (slider.visualPosition * root.to).toFixed(0)

                    onValueChanged: {
                        const value = parseFloat(slider.value.toFixed(2));
                        root.text = value;
                        let obj = {
                            "from": root.from,
                            "to": root.to,
                            "value": value,
                            "type": "slider",
                            "stepSize": root.stepSize
                        };
                        root.save(obj);
                    }

                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: txtSliderValue.left
                        rightMargin: 20
                        left: parent.left
                        leftMargin: 20
                    }
                }

                Text {
                    id: txtSliderValue
                    text: root.text
                    color: Material.foreground
                    horizontalAlignment: Text.AlignRight
                    font.family: App.settings.font

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}
