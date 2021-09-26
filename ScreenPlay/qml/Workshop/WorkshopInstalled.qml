import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
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
