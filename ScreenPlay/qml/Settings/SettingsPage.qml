import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl

Page {
    width: parent.width
    height: contentHeight + header.height + 60
    Material.elevation: 4

    background: Rectangle {
        anchors.fill: parent
        radius: 3
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 4
        }
    }
}
