import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: window

    visible: true
    height: 200
    width: 500

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2)
        setY(Screen.height / 2 - height / 2)
    }

    Text {
        id: name
        text: qsTr("Could not connect to steam Error")
        color: "orange"
        font.pixelSize: 23
        anchors.centerIn: parent
    }
}
