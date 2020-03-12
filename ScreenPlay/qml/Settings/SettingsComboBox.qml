import QtQuick 2.12
import QtQuick.Controls 2.3
import ScreenPlay 1.0

Item {
    id: settingsComboBox
    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property bool enabled: true
    property alias comboBox: comboBox

    height: 20 + txtDescription.paintedHeight + txtHeadline.paintedHeight
    width: parent.width

    Text {
        id: txtHeadline
        color: "#5D5D5D"
        text: settingsComboBox.headline

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors {
            top: parent.top
            topMargin: 6
            left: parent.left
            leftMargin: 20
        }

        font.pointSize: 12
        font.family: ScreenPlay.settings.font
    }

    Text {
        id: txtDescription
        text: settingsComboBox.description
        color: "#B5B5B5"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.WordWrap
        font.pointSize: 10
        font.family: ScreenPlay.settings.font
        anchors {
            top: txtHeadline.bottom
            topMargin: 6
            left: parent.left
            leftMargin: 20
            right: comboBox.left
            rightMargin: 20
        }
    }

    ComboBox {
        id: comboBox
        implicitWidth: 200
        textRole: "text"
        valueRole: "value"
        font.family: ScreenPlay.settings.font
        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }
    }
}
