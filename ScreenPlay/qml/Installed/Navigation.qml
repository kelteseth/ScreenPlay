import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Styles 1.4

import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0
import ScreenPlay.Enums.SearchType 1.0

import "../Common" as Common

Item {
    id: navWrapper
    state: "out"
    height: 52

    Rectangle {
        id: nav
        color: Material.theme === Material.Light ? "white" : Material.background
        height: 50
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 2
        }
    }

    Common.MouseHoverBlocker {}

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

            TabButton {
                text: qsTr("All")
                font.family: ScreenPlay.settings.font
                icon.height: 16
                icon.width: 16
                height: parent.height
                width: implicitWidth
                background: Item {}
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_installed.svg"
                onClicked: {
                    setSidebarActive(false)
                    ScreenPlay.installedListFilter.sortBySearchType(
                                SearchType.All)
                }
            }
            TabButton {
                text: qsTr("Scenes")
                icon.height: 16
                icon.width: 16
                font.family: ScreenPlay.settings.font
                width: implicitWidth
                height: parent.height
                background: Item {}
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_code.svg"
                onClicked: {
                    setSidebarActive(false)
                    ScreenPlay.installedListFilter.sortBySearchType(
                                SearchType.Scene)
                }
            }
            TabButton {
                text: qsTr("Videos")
                icon.height: 16
                icon.width: 16
                font.family: ScreenPlay.settings.font
                height: parent.height
                width: implicitWidth
                background: Item {}
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_movie.svg"
                onClicked: {
                    setSidebarActive(false)
                    ScreenPlay.installedListFilter.sortBySearchType(
                                SearchType.Wallpaper)
                }
            }
            TabButton {
                text: qsTr("Widgets")
                icon.height: 16
                icon.width: 16
                font.family: ScreenPlay.settings.font
                height: parent.height
                width: implicitWidth
                background: Item {}
                font.weight: Font.Thin
                icon.source: "qrc:/assets/icons/icon_widgets.svg"
                onClicked: {
                    setSidebarActive(false)
                    ScreenPlay.installedListFilter.sortBySearchType(
                                SearchType.Widget)
                }
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
            onClicked: {
                sortOrder = (sortOrder
                             === Qt.DescendingOrder) ? Qt.AscendingOrder : Qt.DescendingOrder
                ScreenPlay.installedListFilter.setSortOrder(sortOrder)
            }

            icon.source: (sortOrder === Qt.AscendingOrder) ? "qrc:/assets/icons/icon_sort-down-solid.svg" : "qrc:/assets/icons/icon_sort-up-solid.svg"
            icon.width: 12
            icon.height: 12
            anchors {
                right: parent.right
                rightMargin: 10
                top: parent.top
                verticalCenter: parent.verticalCenter
            }
            hoverEnabled: true

            ToolTip.delay: 100
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: (sortOrder === Qt.AscendingOrder) ? qsTr("Install Date Ascending") : qsTr(
                                                                  "Install Date Descending")
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: navWrapper
                anchors.topMargin: -115
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: navWrapper
                anchors.topMargin: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"

            NumberAnimation {
                target: navWrapper
                property: "anchors.topMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        }
    ]
}
