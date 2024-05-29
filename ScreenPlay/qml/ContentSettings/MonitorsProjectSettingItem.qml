import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects
import ScreenPlayApp

Item {
    id: root

    property int selectedMonitor
    property string name
    property var value
    property bool isHeadline
    property int itemIndex
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
            switch (root.value["type"]) {
            case "slider":
                loader.sourceComponent = compSlider;
                loader.item.from = root.value["from"];
                loader.item.to = root.value["to"];
                loader.item.value = root.value["value"];
                loader.item.stepSize = root.value["stepSize"];
                break;
            case "bool":
                loader.sourceComponent = compCheckbox;
                loader.item.value = root.value["value"];
                break;
            case "color":
                loader.sourceComponent = compColorpicker;
                loader.item.value = root.value["value"];
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
                function onSave(value) {
                    projectSettingsListmodelRef.setValueAtIndex(root.itemIndex, name, value);
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
                    onCheckedChanged: {
                        let obj = {
                            "value": checkbox.checked,
                            "type": "checkBox"
                        };
                        root.save(obj);
                        App.screenPlayManager.setWallpaperValueAtMonitorIndex(selectedMonitor, name, checkbox.checked);
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
                        let tmpColor = "'" + colorDialog.selectedColor.toString() + "'";
                        let obj = {
                            "value": colorDialog.color,
                            "type": "color"
                        };
                        root.save(obj);
                        App.screenPlayManager.setWallpaperValueAtMonitorIndex(selectedMonitor, name, tmpColor);
                    }
                }
            }
        }

        Component {
            id: compSlider

            Item {
                id: root

                property int from
                property int to
                property int value
                property int stepSize: 1

                signal save(var value)

                anchors.fill: parent

                Slider {
                    id: slider

                    from: root.from
                    to: root.to
                    value: root.value
                    stepSize: root.stepSize
                    live: false
                    onValueChanged: {
                        const value = Math.trunc(slider.value * 100) / 100;
                        txtSliderValue.text = value;
                        let obj = {
                            "from": root.from,
                            "to": root.to,
                            "value": value,
                            "type": "slider",
                            "stepSize": root.stepSize
                        };
                        root.save(obj);
                        App.screenPlayManager.setWallpaperValueAtMonitorIndex(selectedMonitor, name, value);
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
