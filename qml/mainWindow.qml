import QtQuick 2.6
import QtQuick.Window 2.2
import QtCanvas3D 1.1
import QtQuick.Controls 2.1



Window {
    id: rectangle
    color: "#eeeeee"
    visible: true
    width: 1366
    height: 768

    Button {
        id: button
        text: qsTr("Close")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        onClicked: Qt.quit();
    }

}
