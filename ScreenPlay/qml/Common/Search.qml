import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import ScreenPlay 1.0

Item {
    id: root

    width: 300

    ToolButton {
        id: icnSearch

        icon.source: "qrc:/assets/icons/icon_search.svg"
        height: 30
        width: 30
        icon.width: 30
        icon.height: 30
        icon.color: Material.iconColor

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

    }

    TextField {
        id: txtSearch

        width: 250
        height: 40
        placeholderText: qsTr("Search for Wallpaper & Widgets")
        onTextChanged: {
            if (txtSearch.text.length === 0)
                ScreenPlay.installedListFilter.resetFilter();
            else
                ScreenPlay.installedListFilter.sortByName(txtSearch.text);
        }

        anchors {
            right: icnSearch.left
            rightMargin: 10
            top: parent.top
            topMargin: 10
        }

    }

}
