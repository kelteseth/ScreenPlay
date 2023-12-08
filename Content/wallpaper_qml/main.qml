// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWallpaper

Rectangle {
    id: root
    color: "#333333"
    anchors.fill: parent
    property int counter: 0
    ColumnLayout {
        anchors.centerIn: parent
        Text {
            Layout.fillWidth: true
            color: "white"
            font.pointSize: 14
            text: "🎉 Qml Button clicked: " + root.counter
            horizontalAlignment: Text.AlignHCenter
        }
        Button {
            text: "Click me!"
            Layout.fillWidth: true
            onClicked: root.counter++
        }
        TextField {
            placeholderText: "Edit me"
        }
        Button {
            text: "Exit"
            onClicked: {
                Qt.callLater(function () {
                        Wallpaper.terminate();
                    });
            }
        }
    }
}
