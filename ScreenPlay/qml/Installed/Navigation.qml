import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0
import ScreenPlay.Enums.SearchType 1.0
import "../Common" as Common

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

    Common.MouseHoverBlocker {
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

            anchors {
                top: parent.top
                topMargin: 5
                left: parent.left
                leftMargin: 20
                bottom: parent.bottom
            }

            TabButton {
                text: qsTr("All")
                font.family: ScreenPlay.settings.font
                icon.height: 16
                icon.width: 16
                height: parent.height
                width: implicitWidth
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_installed.svg"
                onClicked: {
                    setSidebarActive(false);
                    ScreenPlay.installedListFilter.sortBySearchType(SearchType.All);
                }

                background: Item {
                }

            }

            TabButton {
                text: qsTr("Scenes")
                icon.height: 16
                icon.width: 16
                font.family: ScreenPlay.settings.font
                width: implicitWidth
                height: parent.height
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_code.svg"
                onClicked: {
                    setSidebarActive(false);
                    ScreenPlay.installedListFilter.sortBySearchType(SearchType.Scene);
                }

                background: Item {
                }

            }

            TabButton {
                text: qsTr("Videos")
                icon.height: 16
                icon.width: 16
                font.family: ScreenPlay.settings.font
                height: parent.height
                width: implicitWidth
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_movie.svg"
                onClicked: {
                    setSidebarActive(false);
                    ScreenPlay.installedListFilter.sortBySearchType(SearchType.Wallpaper);
                }

                background: Item {
                }

            }

            TabButton {
                text: qsTr("Widgets")
                icon.height: 16
                icon.width: 16
                font.family: ScreenPlay.settings.font
                height: parent.height
                width: implicitWidth
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_widgets.svg"
                onClicked: {
                    setSidebarActive(false);
                    ScreenPlay.installedListFilter.sortBySearchType(SearchType.Widget);
                }

                background: Item {
                }

            }

            background: Item {
            }

        }

        Common.Search {
            height: parent.height

            anchors {
                right: btnSortOrder.left
                rightMargin: 10
                top: parent.top
            }

        }

        ToolButton {
            id: btnSortOrder

            property int sortOrder: Qt.DescendingOrder

            icon.source: (sortOrder === Qt.AscendingOrder) ? "qrc:/assets/icons/icon_sort-down-solid.svg" : "qrc:/assets/icons/icon_sort-up-solid.svg"
            icon.width: 12
            icon.height: 12
            hoverEnabled: true
            ToolTip.delay: 100
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: (sortOrder === Qt.AscendingOrder) ? qsTr("Install Date Ascending") : qsTr("Install Date Descending")
            onClicked: {
                sortOrder = (sortOrder === Qt.DescendingOrder) ? Qt.AscendingOrder : Qt.DescendingOrder;
                ScreenPlay.installedListFilter.setSortOrder(sortOrder);
            }

            anchors {
                right: parent.right
                rightMargin: 10
                top: parent.top
                verticalCenter: parent.verticalCenter
            }

        }

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
