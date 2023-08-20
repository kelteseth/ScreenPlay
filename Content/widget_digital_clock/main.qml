import QtQuick
import QtQuick.Controls.Material
import QtQuick.Window

Item {
    id: root
    implicitWidth: 250
    implicitHeight: 100

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
        txtDate.text = day + ", " + date.getDay() + " " + month + ", " + date.getFullYear();
    }

    Timer {
        interval: 100
        triggeredOnStart: true
        running: true
        repeat: true
        onTriggered: timeChanged()
    }

    Text {
        id: txtClock
        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
        font.pixelSize: 42
        color: "white"
        font.weight: Font.Light
        font.family: "Segoe UI"
    }
    Text {
        id: txtDate
        anchors {
            top: txtClock.bottom
            right: txtClock.right
            bottom: parent.bottom
            left: txtClock.left
        }

        font.pixelSize: 12
        color: "#59C1FE"
        font.weight: Font.Light
        font.family: "Segoe UI"
    }

    MouseArea {
        z: 99
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        height: 15
        width: 15
        onClicked: Qt.quit()
        Image {
            id: imgClose
            opacity: 0
            source: "ic_close_white_64px.svg"
            anchors.fill: parent
            width: parent.width
            height: parent.height
            sourceSize: Qt.size(parent.width, parent.height)
        }
    }

    states: [
        State {
            name: ""
            PropertyChanges {
                target: imgClose
                opacity: 0
            }
        },
        State {
            name: "hover"
            PropertyChanges {
                target: imgClose
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "hover"
            reversible: true

            PropertyAnimation {
                target: imgClose
                property: "opacity"
                duration: 200
            }
        }
    ]
}
