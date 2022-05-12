import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material 2.0
import ScreenPlayApp
import ScreenPlay

Item {
    id: root

    width: 300

    ToolButton {
        id: icnSearch

        icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_search.svg"
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
        placeholderTextColor: Material.secondaryTextColor
        width: 250
        height: 40
        color: Material.secondaryTextColor
        placeholderText: qsTr("Search for Wallpaper & Widgets")
        onTextChanged: {
            if (txtSearch.text.length === 0)
                App.installedListFilter.resetFilter();
            else
                App.installedListFilter.sortByName(txtSearch.text);
        }

        anchors {
            right: icnSearch.left
            rightMargin: 10
            top: parent.top
            topMargin: 10
        }

    }

}
