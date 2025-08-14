import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Window

Item {
    id: root
    implicitWidth: 250
    implicitHeight: 150

    function timeChanged() {
        var date = new Date;
        var hours = "";
        var minutes = "";
        var seconds = "";
        if (date.getHours() < 10) {
            hours = "0" + date.getHours().toString();
        } else {
            hours = date.getHours().toString();
        }
        if (date.getMinutes() < 10) {
            minutes = "0" + date.getMinutes().toString();
        } else {
            minutes = date.getMinutes().toString();
        }
        if (date.getSeconds() < 10) {
            seconds = "0" + date.getSeconds().toString();
        } else {
            seconds = date.getSeconds().toString();
        }
        var days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
        var months = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
        var day = days[date.getDay()];
        var month = months[date.getMonth()];
        txtClock.text = hours + ":" + minutes + ":" + seconds;
        txtDate.text = day + ", " + date.getDate() + " " + month + ", " + date.getFullYear();
    }

    Timer {
        interval: 100
        triggeredOnStart: true
        running: true
        repeat: true
        onTriggered: root.timeChanged()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        Text {
            id: txtClock
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 40
            color: "white"
            font.weight: Font.Light
            font.family: "monospace"
            horizontalAlignment: Text.AlignHCenter
        }
        Text {
            id: txtDate
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 12
            color: Material.color(Material.LightBlue)
            font.weight: Font.Light
            font.family: "monospace"
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
