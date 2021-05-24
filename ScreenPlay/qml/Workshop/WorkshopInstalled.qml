import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import Workshop 1.0 as SP

Item {
    id: pageInstalled

    property bool refresh: false
    property bool enabled: true

    signal setSidebaractiveItem(var screenId, var type)
    signal setNavigationItem(var pos)
    signal setSidebarActive(var active)

    state: "out"
    clip: true
    states: []
    Component.onCompleted: {
        pageInstalled.state = "in";
    }

    Connections {
        function onHelperButtonPressed(pos) {
            setNavigationItem(pos);
        }

        target: loaderHelp.item
    }

    Loader {
        id: loaderHelp

        asynchronous: true
        active: false
        z: 99
        anchors.fill: parent
        source: "qrc:/qml/Installed/InstalledUserHelper.qml"
    }

    transitions: [
        Transition {
            from: "out"
            to: "in"
        }
    ]
}
