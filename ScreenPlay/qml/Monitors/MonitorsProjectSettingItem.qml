import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import ScreenPlay 1.0

Item {
    id: root
    focus: true
    height: isHeadline ? 50 : 30
    property int selectedMonitor
    property string name
    property var value
    property bool isHeadline

    Text {
        id: txtDescription
        text: root.name
        width: 100
        font.pointSize: root.isHeadline ? 18 : 12
        anchors.verticalCenter: parent.verticalCenter
        font.family: ScreenPlay.settings.font
        font.weight: Font.Normal
        color: root.isHeadline ? Qt.darker(
                                     Material.foreground) : Material.foreground

        anchors {
            left: parent.left
            leftMargin: root.isHeadline ? 0 : 25
        }
    }

    Item {
        height: parent.height
        visible: !root.isHeadline
        anchors {
            left: txtDescription.right
            leftMargin: 20
            right: parent.right
        }

        Component.onCompleted: {
            if (root.value.toString() === "") {
                return
            }

            if (root.isHeadline)
                return


            for (let item in root.value) {
               // print(item.toString())
                switch (item["type"]) {
                case "slider":
                    loader.sourceComponent = compSlider
                    loader.item.from = obj["from"]
                    loader.item.to = obj["to"]
                    loader.item.value = obj["value"]
                    loader.item.stepSize = obj["stepSize"]
                    break
                case "bool":
                    loader.sourceComponent = compCheckbox
                    loader.item.value = obj["value"]
                    break
                case "color":
                    loader.sourceComponent = compColorpicker
                    loader.item.value = obj["value"]
                    break
                }

                if (item["text"]) {
                    txtDescription.text = obj["text"]
                }
            }
        }

        Component {
            id: compCheckbox

            Item {
                id: root
                anchors.fill: parent
                property bool value

                CheckBox {
                    id: checkbox
                    checkable: true
                    checked: root.value
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    onCheckedChanged: {
                        ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                                    selectedMonitor, name, checkbox.checked)
                    }
                }
            }
        }

        Component {
            id: compColorpicker

            Item {
                id: root
                anchors.fill: parent
                property color value

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
                        rctPreviewColor.color = colorDialog.color
                        let tmpColor = "'" + colorDialog.color.toString() + "'"
                        ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                                    selectedMonitor, name, tmpColor)
                    }
                }
            }
        }

        Component {
            id: compSlider

            Item {
                id: root
                anchors.fill: parent
                property int from
                property int to
                property int value
                property int stepSize

                Slider {
                    id: slider
                    from: root.from
                    to: root.to
                    value: root.value
                    stepSize: root.stepSize
                    live: false

                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: txtSliderValue.left
                        rightMargin: 20
                        left: parent.left
                        leftMargin: 20
                    }

                    onValueChanged: {
                        var value = Math.round(slider.value * 100) / 100
                        txtSliderValue.text = value
                        ScreenPlay.screenPlayManager.setWallpaperValueAtMonitorIndex(
                                    selectedMonitor, name, value)
                    }
                }
                Text {
                    id: txtSliderValue
                    color: Material.foreground
                    horizontalAlignment: Text.AlignRight
                    font.family: ScreenPlay.settings.font
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Loader {
            id: loader
            anchors.fill: parent
            anchors.rightMargin: 10
        }
    }
}

/*##^##
Designer {
    D{i:0;height:50;width:400}
}
##^##*/

