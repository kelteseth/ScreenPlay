import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0
import ScreenPlay.Enums.SearchType 1.0

Item {
    id: pageInstalled

    signal setNavigationItem(var pos)
    signal setSidebarActive(var active)

    property bool refresh: false
    property bool enabled: true

    Component.onCompleted: {
        navWrapper.state = "in"
        ScreenPlay.installedListFilter.sortBySearchType(SearchType.All)
        checkIsContentInstalled()
    }

    Action {
        shortcut: "F5"
        onTriggered: ScreenPlay.installedListModel.reset()
    }

    Connections {
        target: loaderHelp.item
        function onHelperButtonPressed(pos) {
            setNavigationItem(pos)
        }
    }

    Connections {
        target: ScreenPlay.installedListModel
        function onInstalledLoadingFinished() {
            checkIsContentInstalled()
        }
        function onCountChanged(count) {
            if (count === 0) {
                checkIsContentInstalled()
            }
        }
    }

    function checkIsContentInstalled() {
        if (ScreenPlay.installedListModel.count === 0) {
            loaderHelp.active = true
            gridView.footerItem.isVisible = true
            gridView.visible = false
            navWrapper.visible = false
        } else {
            loaderHelp.active = false
            gridView.footerItem.isVisible = false
            refresh = false
            gridView.contentY = -82
            gridView.visible = true
            navWrapper.visible = true
        }
    }

    Loader {
        id: loaderHelp
        active: false
        z: 99
        anchors.fill: parent
        source: "qrc:/qml/Installed/InstalledWelcomeScreen.qml"
    }

    GridView {
        id: gridView
        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 2500
        flickDeceleration: 2500
        anchors.fill: parent
        cellWidth: 340
        cellHeight: 200
        interactive: pageInstalled.enabled
        anchors {
            topMargin: 0
            rightMargin: 0
            leftMargin: 30
        }
        header: Item {
            height: 82
            width: parent.width
            property bool isVisible: false
            opacity: 0
            onIsVisibleChanged: {
                if (isVisible) {
                    txtHeader.color = Material.accent
                    txtHeader.text = qsTr("Refreshing!")
                } else {
                    txtHeader.color = "gray"
                    txtHeader.text = qsTr("Pull to refresh!")
                }
            }

            Timer {
                interval: 150
                running: true
                onTriggered: {
                    animFadeIn.start()
                }
            }

            PropertyAnimation on opacity {
                id: animFadeIn
                from: 0
                to: 1
                running: false
                duration: 1000
            }

            Text {
                id: txtHeader
                text: qsTr("Pull to refresh!")
                font.family: ScreenPlay.settings.font
                anchors.centerIn: parent
                color: "gray"
                font.pointSize: 18
            }
        }
        footer: Item {
            property bool isVisible: true
            height: 100
            opacity: 0
            visible: isVisible
            width: parent.width

            Text {
                id: txtFooter
                font.family: ScreenPlay.settings.font
                text: qsTr("Get more Wallpaper & Widgets via the Steam workshop!")
                anchors.centerIn: parent
                color: "gray"

                Timer {
                    interval: 400
                    running: true
                    onTriggered: {
                        animFadeInTxtFooter.start()
                    }
                }

                PropertyAnimation on opacity {
                    id: animFadeInTxtFooter
                    from: 0
                    to: 1
                    running: false
                    duration: 1000
                }
            }
        }
        property bool isDragging: false
        onDragStarted: isDragging = true
        onDragEnded: isDragging = false
        onContentYChanged: {
            if (contentY <= -180) {
                gridView.headerItem.isVisible = true
            } else {
                gridView.headerItem.isVisible = false
            }

            //Pull to refresh
            if (contentY <= -180 && !refresh && !isDragging) {
                ScreenPlay.installedListModel.reset()
            }
        }

        model: ScreenPlay.installedListFilter

        delegate: ScreenPlayItem {
            id: delegate
            focus: true

            customTitle: screenTitle
            type: screenType
            screenId: screenFolderId
            absoluteStoragePath: screenAbsoluteStoragePath
            workshopID: screenWorkshopID
            itemIndex: index
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }

    Navigation {
        id: navWrapper
        height: 115
        width: parent.width
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }
}
