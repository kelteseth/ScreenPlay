import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    id: settingsBool
    property string name: ""
    property bool isChecked: false
    signal checkboxChanged(bool checked)


    height: 30

    Text {
        id: txtName
        text: settingsBool.name
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: 16
        font.family: font_Roboto_Regular.name

        FontLoader {
            id: font_Roboto_Regular
            source: "qrc:/assets/fonts/Roboto-Regular.ttf"
        }
    }

    CheckBox {
        id: radioButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: txtName.right
        anchors.leftMargin: 20
        checked: settingsBool.isChecked
        onCheckedChanged: checkboxChanged(radioButton.checked)
    }
}
