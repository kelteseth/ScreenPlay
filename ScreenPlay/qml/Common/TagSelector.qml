import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.12

Item {
    id: tagSelector
    height: 70
    onStateChanged: {
        if (tagSelector.state === "add") {
            btnAdd.text = qsTr("Save")
            textField.focus = true
        } else {
            btnAdd.text = qsTr("Add tag")
        }
    }

    function getTags(){
        var array = [];
        for(var i = 0; i < listModel.count; i++) {
            array.push(listModel.get(i)._name)
        }
        return array
    }

    Rectangle {
        id: rectangle
        color: "#F0F0F0"
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
                    onRemoveThis: {
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
            color: "#aaffffff"
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
                anchors.fill: parent
                anchors.rightMargin: 15
                anchors.leftMargin: 15
                onTextChanged: {
                    if(textField.length >= 10){
                        textField.text = textField.text
                    }
                }
            }
        }

        Button {
            id: btnCancel
            text: qsTr("Cancel")
            opacity: 0
            enabled: false
            Material.background: Material.Red
            Material.foreground: "white"
            anchors {
                right: btnAdd.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            onClicked: {
                tagSelector.state = ""
                textField.clear()
            }
        }
        Button {
            id: btnAdd
            text: qsTr("Add Tag")
            Material.background: Material.LightGreen
            Material.foreground: "white"
            anchors {
                right: parent.right
                rightMargin: 20
                verticalCenter: parent.verticalCenter
            }

            onClicked: {
                if (tagSelector.state === "add") {
                    listModel.append({
                                         "_name": textField.text
                                     })
                    textField.clear()
                    tagSelector.state = ""
                } else {
                    tagSelector.state = "add"
                }
            }
        }
    }

    states: [

        State {
            name: "add"

            PropertyChanges {
                target: textFieldWrapper
                color: "#ccffffff"
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
