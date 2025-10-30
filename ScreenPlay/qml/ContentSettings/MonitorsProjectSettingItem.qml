import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
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
                return
            if (!root.value || typeof root.value !== "object") {
                console.warn(LoggingCategories.monitorProjectSettings, "Invalid value object for:", root.name)
                return
            }

            const valueType = root.value["type"]
            if (!valueType) {
                console.warn(LoggingCategories.monitorProjectSettings, "Missing type for:", root.name)
                return
            }

            console.log(LoggingCategories.monitorProjectSettings, "MonitorsProjectSettingItem:", valueType)

            switch (valueType) {
            case "slider":
                loader.sourceComponent = compSlider
                break
            case "bool":
                loader.sourceComponent = compCheckbox
                break
            case "color":
                loader.sourceComponent = compColorpicker
                break
            case "file":
                loader.sourceComponent = compFilepicker
                break
            default:
                console.error(LoggingCategories.monitorProjectSettings, root.name, " has an invalid type:", valueType)
                break
            }
            if (root.value["text"])
                txtDescription.text = root.value["text"]
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

            onLoaded: {
                if (!root.value || typeof root.value !== "object")
                    return
                const valueType = root.value["type"]
                const loadedItem = item

                switch (valueType) {
                case "slider":
                    loadedItem.from = root.value["from"] !== undefined ? root.value["from"] : 0
                    loadedItem.to = root.value["to"] !== undefined ? root.value["to"] : 100
                    loadedItem.value = root.value["value"] !== undefined ? root.value["value"] : 0
                    loadedItem.stepSize = root.value["stepSize"] !== undefined ? root.value["stepSize"] : 1
                    loadedItem.text = parseFloat(loadedItem.value.toFixed(2))
                    break
                case "bool":
                    loadedItem.value = root.value["value"] !== undefined ? root.value["value"] : false
                    break
                case "color":
                    loadedItem.value = root.value["value"] !== undefined ? root.value["value"] : "#ffffff"
                    break
                case "file":
                    loadedItem.value = root.value["value"] !== undefined ? root.value["value"] : ""
                    break
                }
            }

            Connections {
                function onSave(obj) {
                    console.log(LoggingCategories.monitorProjectSettings, "on save value:", root.index, root.name, root.category, obj.value)
                    root.projectSettingsListmodelRef.setValueAtIndex(root.index, root.name, root.category, obj.value)

                    const monitorIndex = root.selectedMonitorIndex
                    const timelineIndex = root.timelineIndex
                    const sectionIdentifier = root.sectionIdentifier
                    const key = root.name
                    const value = obj.value

                    App.screenPlayManager.setValueAtMonitorTimelineIndex(monitorIndex, timelineIndex, sectionIdentifier, key, value, root.category).then(result => {
                        settingValue = false
                        if (!result.success) {
                            InstantPopup.openErrorPopup(root, result.message)
                        }
                    })
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
                        }
                        root.save(obj)
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
                        rctPreviewColor.color = colorDialog.selectedColor
                        let obj = {
                            "value": colorDialog.selectedColor.toString(),
                            "type": "color"
                        }
                        root.save(obj)
                    }
                }
            }
        }

        Component {
            id: compFilepicker

            Item {
                id: root

                property string value

                signal save(var value)

                anchors.fill: parent

                Button {
                    id: btnSelectFile

                    text: qsTr("Select file")
                    onClicked: fileDialog.open()

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }

                Text {
                    id: txtFilePath

                    text: root.value || qsTr("No file selected")
                    color: Material.foreground
                    elide: Text.ElideMiddle
                    font.family: App.settings.font

                    anchors {
                        left: parent.left
                        right: btnSelectFile.left
                        rightMargin: 20
                        verticalCenter: parent.verticalCenter
                    }
                }

                FileDialog {
                    id: fileDialog

                    title: qsTr("Please choose a file")
                    onAccepted: {
                        txtFilePath.text = fileDialog.selectedFile
                        let obj = {
                            "value": fileDialog.selectedFile,
                            "type": "file"
                        }
                        root.save(obj)
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
                        const value = parseFloat(slider.value.toFixed(2))
                        root.text = value
                        let obj = {
                            "from": root.from,
                            "to": root.to,
                            "value": value,
                            "type": "slider",
                            "stepSize": root.stepSize
                        }
                        root.save(obj)
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
