import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2


Item {
    id: settingsButton

    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property string buttonText: "value"
    property bool isChecked: false
    property bool enabled: true
    signal buttonPressed()

    height: 50
    width: parent.width

    Text {
        id: txtHeadline
        color: "#5D5D5D"
        text: settingsButton.headline
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
    }

    Text {
        id: txtDescription
        text: settingsButton.description
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


    Button {
        text: settingsButton.buttonText
        Material.background: Material.Orange
        Material.foreground: "white"
        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }
        onPressed: buttonPressed()

    }
}
