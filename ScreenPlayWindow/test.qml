import QtQuick 2.12
import QtQuick.Controls.Material 2.12

Rectangle {
    anchors.fill: parent
    color: Material.color(Material.Grey, Material.Shade800)

    Text {
        id: name
        text: qsTr("This is a empty test window. You can change the source in test.qml")
        font.pointSize: 32
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        anchors.fill: parent
        anchors.margins: 10
        color: "white"
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/

