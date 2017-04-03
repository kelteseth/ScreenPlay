import QtQuick 2.6
import QtQuick.Window 2.2
import QtCanvas3D 1.1
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "Components"

Window {
    id: rectangle
    color: "#eeeeee"
    visible: true
    width: 1366
    height: 768
    Component.onCompleted: installedListModel.loadDrives()



    Loader {
        id: pageLoader
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        source: "qrc:/qml/Components/Installed.qml"
    }

    Sidebar {
        id: sidebar
        width:400
        anchors {
            top:parent.top
            right:parent.right
            bottom:parent.bottom
        }

    }

    Navigation {
        id: nav
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        onChangeTab: pageLoader.setSource("qrc:/qml/Components/"+name+".qml")
    }
}
