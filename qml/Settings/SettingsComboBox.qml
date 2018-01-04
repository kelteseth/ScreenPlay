import QtQuick 2.9
import QtQuick.Controls 2.3


Item {
    id: settingsComboBox
    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property int currentIndex:0
    property bool isChecked: false
    property bool enabled: true
    property ListModel comboBoxListModel

    onEnabledChanged: {

    }

    signal checkboxChanged(bool checked)


    height: 50
    width: parent.width

    Text {
        id: txtHeadline
        color: "#5D5D5D"
        text: settingsComboBox.headline
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors{
            top:parent.top
            topMargin: 6
            left:parent.left
            leftMargin: 20

        }

       font.pixelSize: 16
       font.family: "Roboto"
    }

    Text {
        id: txtDescription
        text: settingsComboBox.description
        color: "#B5B5B5"
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 12
        font.family: "Roboto"
        anchors{
            top:txtHeadline.bottom
            topMargin: 6
            left:parent.left
            leftMargin: 20
        }
    }


    ComboBox {
        id:comboBox

        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }

        model: comboBoxListModel
        onActivated:settingsComboBox.currentIndex = currentIndex
    }
}
