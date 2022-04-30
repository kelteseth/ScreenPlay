import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import ScreenPlayApp 1.0
import ScreenPlay 1.0

Item {
    id: root

    function getTags() {
        var array = []
        for (var i = 0; i < listModel.count; i++) {
            array.push(listModel.get(i)._name)
        }
        return array
    }

    height: 70
    implicitWidth: 200
    onStateChanged: {
        if (root.state === "add") {
            btnAdd.text = qsTr("Save")
            textField.focus = true
        } else {
            btnAdd.text = qsTr("Add tag")
        }
    }

    Rectangle {
        id: rectangle

        color: Material.theme === Material.Light ? Material.background : Qt.darker(
                                                       Material.background)
        radius: 3
        clip: true

        anchors {
            fill: parent
            margins: 3
        }

        ListView {
            orientation: ListView.Horizontal
            model: listModel
            spacing: 10

            anchors {
                top: parent.top
                right: btnAdd.left
                bottom: parent.bottom
                left: parent.left
                margins: 10
            }

            delegate: Tag {
                id: delegate

                text: _name
                itemIndex: index

                Connections {
                    function onRemoveThis() {
                        listModel.remove(itemIndex)
                    }

                    target: delegate
                }
            }
        }

        ListModel {
            id: listModel

            onCountChanged: getTags()
        }

        Rectangle {
            id: textFieldWrapper

            opacity: 0
            enabled: false
            radius: 3
            height: parent.height - 20
            width: 200
            color: Material.theme
                   === Material.Light ? Qt.lighter(
                                            Material.background) : Material.background

            anchors {
                top: parent.top
                topMargin: -80
                right: btnCancel.left
                margins: 10
            }

            Gradient {
                GradientStop {
                    position: 0
                    color: "#00000000"
                }

                GradientStop {
                    position: 1
                    color: "#FF000000"
                }
            }

            TextField {
                id: textField

                font.family: App.settings.font
                color: Material.primaryTextColor
                onTextChanged: {
                    if (textField.length >= 10)
                        textField.text = textField.text
                }

                anchors {
                    fill: parent
                    rightMargin: 15
                    leftMargin: 15
                }
            }
        }

        Button {
            id: btnCancel

            text: qsTr("Cancel")
            opacity: 0
            height: parent.height - 20
            enabled: false
            Material.accent: Material.color(Material.Red)
            highlighted: true
            font.family: App.settings.font
            onClicked: {
                root.state = ""
                textField.clear()
            }

            anchors {
                right: btnAdd.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
        }

        Button {
            id: btnAdd

            text: qsTr("Add Tag")
            height: parent.height - 20
            Material.accent: Material.color(Material.LightGreen)
            highlighted: true
            font.family: App.settings.font
            onClicked: {
                if (root.state === "add") {
                    listModel.append({
                                         "_name": textField.text
                                     })
                    textField.clear()
                    root.state = ""
                } else {
                    root.state = "add"
                }
            }

            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
        }
    }

    states: [
        State {
            name: "add"

            PropertyChanges {
                target: textFieldWrapper
                anchors.topMargin: 10
                opacity: 1
                enabled: true
            }

            PropertyChanges {
                target: btnCancel
                opacity: 1
                enabled: true
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "add"
            reversible: true

            NumberAnimation {
                properties: "anchors.topMargin, opacity"
                duration: 200
                easing.type: Easing.OutQuart
            }
        }
    ]
}
