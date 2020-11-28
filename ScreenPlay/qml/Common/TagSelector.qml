import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.12

import ScreenPlay 1.0

Item {
    id: root
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

    function getTags() {
        var array = []
        for (var i = 0; i < listModel.count; i++) {
            array.push(listModel.get(i)._name)
        }
        return array
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
                    target: delegate
                    function onRemoveThis() {
                        listModel.remove(itemIndex)
                    }
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
                    position: 0.0
                    color: "#00000000"
                }
                GradientStop {
                    position: 1.0
                    color: "#FF000000"
                }
            }

            TextField {
                id: textField
                font.family: ScreenPlay.settings.font
                color: Material.primaryTextColor
                anchors {
                    fill: parent
                    rightMargin: 15
                    leftMargin: 15
                }

                onTextChanged: {
                    if (textField.length >= 10) {
                        textField.text = textField.text
                    }
                }
            }
        }

        Button {
            id: btnCancel
            text: qsTr("Cancel")
            opacity: 0
            height: parent.height - 20
            enabled: false
            Material.background: Material.Red
            Material.foreground: "white"
            font.family: ScreenPlay.settings.font
            anchors {
                right: btnAdd.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            onClicked: {
                root.state = ""
                textField.clear()
            }
        }
        Button {
            id: btnAdd
            text: qsTr("Add Tag")
            height: parent.height - 20
            Material.background: Material.LightGreen
            Material.foreground: "white"
            font.family: ScreenPlay.settings.font
            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

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
