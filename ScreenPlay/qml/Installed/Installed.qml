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
    id: root

    property bool refresh: false
    property bool enabled: true
    property Sidebar sidebar

    signal setNavigationItem(var pos)
    signal setSidebarActive(var active)

    function checkIsContentInstalled() {
        if (ScreenPlay.installedListModel.count === 0) {
            loaderHelp.active = true;
            gridView.footerItem.isVisible = true;
            gridView.visible = false;
            navWrapper.visible = false;
        } else {
            loaderHelp.active = false;
            gridView.footerItem.isVisible = false;
            refresh = false;
            gridView.contentY = -82;
            gridView.visible = true;
            navWrapper.visible = true;
        }
    }

    Component.onCompleted: {
        navWrapper.state = "in";
        ScreenPlay.installedListFilter.sortBySearchType(SearchType.All);
        checkIsContentInstalled();
    }

    Action {
        shortcut: "F5"
        onTriggered: ScreenPlay.installedListModel.reset()
    }

    Connections {
        function onHelperButtonPressed(pos) {
            setNavigationItem(pos);
        }

        target: loaderHelp.item
    }

    Connections {
        function onInstalledLoadingFinished() {
            checkIsContentInstalled();
        }

        function onCountChanged(count) {
            if (count === 0)
                checkIsContentInstalled();

        }

        target: ScreenPlay.installedListModel
    }

    Loader {
        id: loaderHelp

        active: false
        z: 99
        anchors.fill: parent
        source: "qrc:/qml/Installed/InstalledWelcomeScreen.qml"
    }

    Connections {
        function onSortChanged() {
            gridView.positionViewAtBeginning();
        }

        target: ScreenPlay.installedListFilter
    }

    GridView {
        id: gridView
        objectName: "gridView"

        property bool isDragging: false
        property bool isScrolling: gridView.verticalVelocity != 0

        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 2500
        flickDeceleration: 500
        anchors.fill: parent
        cellWidth: 340
        cellHeight: 200
        cacheBuffer: 160
        interactive: root.enabled
        snapMode: GridView.SnapToRow
        onDragStarted: isDragging = true
        onDragEnded: isDragging = false
        model: ScreenPlay.installedListFilter
        removeDisplaced: Transition {
            SequentialAnimation {
                PauseAnimation {
                    duration: 150
                }
                NumberAnimation {
                    properties: "x,y"
                    duration: 250
                    easing.type: Easing.InOutQuart
                }
            }
        }

        remove: Transition {
            SequentialAnimation {

                NumberAnimation {
                    property: "opacity"
                    to: 0
                    duration: 200
                    easing.type: Easing.InOutQuart
                }
                NumberAnimation {
                    properties: "y"
                    to: 100
                    duration: 200
                    easing.type: Easing.InOutQuart
                }
            }
        }
        onContentYChanged: {
            if (contentY <= -180)
                gridView.headerItem.isVisible = true;
            else
                gridView.headerItem.isVisible = false;
            //Pull to refresh
            if (contentY <= -180 && !refresh && !isDragging)
                ScreenPlay.installedListModel.reset();

        }

        anchors {
            topMargin: 0
            rightMargin: 0
            leftMargin: 30
        }

        header: Item {
            property bool isVisible: false

            height: 82
            width: parent.width - gridView.leftMargin
            opacity: 0
            onIsVisibleChanged: {
                if (isVisible) {
                    txtHeader.color = Material.accent;
                    txtHeader.text = qsTr("Refreshing!");
                } else {
                    txtHeader.color = "gray";
                    txtHeader.text = qsTr("Pull to refresh!");
                }
            }

            Timer {
                interval: 150
                running: true
                onTriggered: {
                    animFadeIn.start();
                }
            }

            Text {
                id: txtHeader

                text: qsTr("Pull to refresh!")
                font.family: ScreenPlay.settings.font
                anchors.centerIn: parent
                color: "gray"
                font.pointSize: 18
            }

            PropertyAnimation on opacity {
                id: animFadeIn

                from: 0
                to: 1
                running: false
                duration: 1000
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
                        animFadeInTxtFooter.start();
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

        delegate: ScreenPlayItem {
            id: delegate
            objectName: "installedItem" + index
            focus: true
            customTitle: m_title
            type: m_type
            isNew: m_isNew
            screenId: m_folderId
            absoluteStoragePath: m_absoluteStoragePath
            publishedFileID: m_publishedFileID
            itemIndex: index
            isScrolling: gridView.isScrolling
            onOpenContextMenu: {
                // Set the menu to the current item informations
                contextMenu.publishedFileID = delegate.publishedFileID;
                contextMenu.absoluteStoragePath = delegate.absoluteStoragePath;
                const pos = delegate.mapToItem(root, position.x, position.y);
                // Disable duplicate opening. The can happen if we
                // call popup when we are in the closing animtion.
                if (contextMenu.visible || contextMenu.opened)
                    return ;

                contextMenu.popup(pos.x, pos.y);
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }

    }

    Menu {
        id: contextMenu
        objectName: "installedItemContextMenu"

        property var publishedFileID: 0
        property url absoluteStoragePath

        MenuItem {
            text: qsTr("Open containing folder")
            objectName: "openFolder"
            icon.source: "qrc:/assets/icons/icon_folder_open.svg"
            onClicked: {
                ScreenPlay.util.openFolderInExplorer(
                            contextMenu.absoluteStoragePath)
            }
        }

        MenuItem {
            text: enabled ? qsTr("Remove Item") : qsTr("Remove via Workshop")
            objectName: enabled ? "removeItem" : "removeWorkshopItem"
            icon.source: "qrc:/assets/icons/icon_delete.svg"
            enabled: contextMenu.publishedFileID === 0
            onClicked: {
                deleteDialog.open()
            }
        }

        MenuItem {
            text: qsTr("Open Workshop Page")
            enabled: contextMenu.publishedFileID !== 0
            icon.source: "qrc:/assets/icons/icon_steam.svg"
            onClicked: {
                Qt.openUrlExternally(
                            "steam://url/CommunityFilePage/" + contextMenu.publishedFileID)
            }
        }
    }

    Dialog {
        id: deleteDialog
        title: qsTr("Are you sure you want to delete this item?")
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true
        dim: true
        anchors.centerIn: Overlay.overlay
        onAccepted: {
            root.sidebar.clear()
            ScreenPlay.installedListModel.deinstallItemAt(
                        contextMenu.absoluteStoragePath)
        }
    }

    Navigation {
        id: navWrapper

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }
}
