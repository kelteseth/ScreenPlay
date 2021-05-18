import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12

Page {
    width: parent.width
    height: contentHeight + header.height + 30 * 3
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
