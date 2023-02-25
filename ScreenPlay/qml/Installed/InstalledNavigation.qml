import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Enums.InstalledType
import ScreenPlay.Enums.SearchType
import ScreenPlayUtil as Util

Item {
    id: root

    state: "out"
    height: 52

    Rectangle {
        id: nav

        color: Material.theme === Material.Light ? "white" : Material.background
        height: 50
        layer.enabled: true

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        layer.effect: ElevationEffect {
            elevation: 2
        }
    }

    Util.MouseHoverBlocker {
    }

    Item {
        height: nav.height

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        TabBar {
            height: parent.height

            background: Item {}
            anchors {
                top: parent.top
                topMargin: 5
                left: parent.left
                leftMargin: 20
                bottom: parent.bottom
            }

            CustomTabButton {
                text: qsTr("All")
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_installed.svg"
                onClicked: {
                    setSidebarActive(false);
                    App.installedListFilter.sortBySearchType(SearchType.All);
                }
            }

            CustomTabButton {
                text: qsTr("Scenes")
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_code.svg"
                onClicked: {
                    setSidebarActive(false);
                    App.installedListFilter.sortBySearchType(SearchType.Scene);
                }
            }

            CustomTabButton {
                text: qsTr("Videos")
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_movie.svg"
                onClicked: {
                    setSidebarActive(false);
                    App.installedListFilter.sortBySearchType(SearchType.Wallpaper);
                }
            }

            CustomTabButton {
                text: qsTr("Widgets")
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_widgets.svg"
                onClicked: {
                    setSidebarActive(false);
                    App.installedListFilter.sortBySearchType(SearchType.Widget);
                }
            }
        }

        RowLayout {
            anchors {
                right: btnSortOrder.left
                rightMargin: 10
                top: parent.top
                bottom: parent.bottom
            }

            Text {
                text: qsTr("Search:")
                color: Material.secondaryTextColor
                font.pointSize: 12
                font.family: App.settings.font
                Layout.alignment: Qt.AlignVCenter
                Layout.rightMargin: 10
            }

            TextField {
                id: txtSearch
                // Workaround with manual Text until
                // https://bugreports.qt.io/browse/QTBUG-111515 is fixed
                // placeholderTextColor: Material.secondaryTextColor
                // placeholderText: qsTr("Search for Wallpaper & Widgets")
                Layout.preferredHeight: 30
                Layout.preferredWidth : 250
                Layout.alignment: Qt.AlignVCenter
                color: Material.secondaryTextColor
                onTextChanged: {
                    if (txtSearch.text.length === 0)
                        App.installedListFilter.resetFilter();
                    else
                        App.installedListFilter.sortByName(txtSearch.text);
                }

            }
            ToolButton {
                id: icnSearch

                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_search.svg"
                implicitHeight: 30
                implicitWidth: height
                icon.width: height
                icon.height: height
                icon.color: Material.iconColor
            }
 
        }

        ToolButton {
            id: btnSortOrder

            property int sortOrder: Qt.DescendingOrder

            icon.source: (btnSortOrder.sortOrder === Qt.AscendingOrder) ? "qrc:/qml/ScreenPlayApp/assets/icons/icon_sort-down-solid.svg" : "qrc:/qml/ScreenPlayApp/assets/icons/icon_sort-up-solid.svg"
            icon.width: 12
            icon.height: 12
            hoverEnabled: true
            ToolTip.delay: 100
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: (btnSortOrder.sortOrder === Qt.AscendingOrder) ? "Install Date Ascending" : "Install Date Descending"
            onClicked: {
                btnSortOrder.sortOrder = (btnSortOrder.sortOrder === Qt.DescendingOrder) ? Qt.AscendingOrder : Qt.DescendingOrder;
                App.installedListFilter.setSortOrder(btnSortOrder.sortOrder);
            }

            anchors {
                right: parent.right
                rightMargin: 10
                top: parent.top
                verticalCenter: parent.verticalCenter
            }
        }
    }

    component CustomTabButton: TabButton {
        icon.height: 16
        icon.width: 16
        height: parent.height
        width: implicitWidth
        background: Item {
        }
        font.capitalization: Font.MixedCase
    }

    states: [
        State {
            name: "out"

            PropertyChanges {
                target: root
                anchors.topMargin: -115
            }
        },
        State {
            name: "in"

            PropertyChanges {
                target: root
                anchors.topMargin: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"

            NumberAnimation {
                target: root
                property: "anchors.topMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        }
    ]
}
