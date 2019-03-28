import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2


Item {
    id: settingsButton

    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property string buttonText: "value"
    property bool isChecked: false
    property bool enabled: true
    property bool available: true
    onAvailableChanged: {
        if(!available){
            settingsButton.opacity = .5
            btnSettings.enabled = false
        } else {
            settingsButton.opacity = 1
            btnSettings.enabled = true
        }
    }

    signal buttonPressed()

    height: 50
    width: parent.width

    Text {
        id: txtHeadline
        color: "#5D5D5D"
        text: settingsButton.headline
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors{
            top:parent.top
            topMargin: 6
            left:parent.left
            leftMargin: 20

        }

       font.pointSize: 12
    }

    Text {
        id: txtDescription
        text: settingsButton.description
        color: "#B5B5B5"
        
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        font.pointSize: 10
        font.family: "Roboto"
        anchors{
            top:txtHeadline.bottom
            topMargin: 6
            left:parent.left
            leftMargin: 20
            right:btnSettings.left
            rightMargin: 20
        }
    }


    Button {
        id:btnSettings
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
