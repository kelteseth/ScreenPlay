import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import Qt.labs.platform 1.1 as Labs
import ScreenPlayApp
import ScreenPlay
import ScreenPlay.Enums.InstalledType
import ScreenPlay.Enums.SearchType
import ScreenPlayUtil as Util

Item {
    id: root

    property bool refresh: false
    property bool enabled: true

    property Sidebar sidebar

    signal setNavigationItem(var pos)
    signal setSidebarActive(var active)

    function checkIsContentInstalled() {
        if (App.installedListModel.count === 0) {
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

    Component.onCompleted: {
        navWrapper.state = "in"
        App.installedListFilter.sortBySearchType(SearchType.All)
        checkIsContentInstalled()
    }

    Action {
        shortcut: "F5"
        onTriggered: App.installedListModel.reset()
    }

    Connections {
        function onHelperButtonPressed(pos) {
            setNavigationItem(pos)
        }

        target: loaderHelp.item
    }

    Connections {
        function onInstalledLoadingFinished() {
            checkIsContentInstalled()
        }

        function onCountChanged(count) {
            if (count === 0)
                checkIsContentInstalled()
        }

        target: App.installedListModel
    }

    Loader {
        id: loaderHelp

        active: false
        z: 99
        anchors.fill: parent
        source: "qrc:/qml/ScreenPlayApp/qml/Installed/InstalledWelcomeScreen.qml"
    }

    Connections {
        function onSortChanged() {
            gridView.positionViewAtBeginning()
        }

        target: App.installedListFilter
    }

    GridView {
        id: gridView
        objectName: "gridView"

        property bool isDragging: false
        property bool isScrolling: gridView.verticalVelocity !== 0

        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 5000
        flickDeceleration: 5000
        anchors.fill: parent
        cellWidth: 340
        cellHeight: 200
        cacheBuffer: 160
        interactive: root.enabled
        snapMode: GridView.SnapToRow
        onDragStarted: isDragging = true
        onDragEnded: isDragging = false
        model: App.installedListFilter
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
                gridView.headerItem.isVisible = true
            else
                gridView.headerItem.isVisible = false
            //Pull to refresh
            if (contentY <= -180 && !refresh && !isDragging)
                App.installedListModel.reset()
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

            Text {
                id: txtHeader

                text: qsTr("Pull to refresh!")
                font.family: App.settings.font
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

                font.family: App.settings.font
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
            onOpenContextMenu: function (position) {
                // Set the menu to the current item informations
                contextMenu.publishedFileID = delegate.publishedFileID
                contextMenu.absoluteStoragePath = delegate.absoluteStoragePath
                contextMenu.fileName = delegate.customTitle
                const pos = delegate.mapToItem(root, position.x, position.y)
                // Disable duplicate opening. The can happen if we
                // call popup when we are in the closing animtion.
                if (contextMenu.visible || contextMenu.opened)
                    return

                contextMenu.popup(pos.x, pos.y)
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
        property string fileName

        MenuItem {
            text: qsTr("Open containing folder")
            objectName: "openFolder"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_folder_open.svg"
            onClicked: {
                App.util.openFolderInExplorer(contextMenu.absoluteStoragePath)
            }
        }

        MenuItem {
            text: qsTr("Export")
            objectName: enabled ? "removeItem" : "removeWorkshopItem"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_download.svg"
            onClicked: {
                exportFileDialog.absoluteStoragePath = contextMenu.absoluteStoragePath
                let urlFileName = Labs.StandardPaths.writableLocation(
                        Labs.StandardPaths.DesktopLocation) + "/"
                    + contextMenu.fileName + ".screenplay"

                exportFileDialog.currentFile = urlFileName
                exportFileDialog.open()
            }
        }

        MenuItem {
            text: enabled ? qsTr("Remove Item") : qsTr("Remove via Workshop")
            objectName: enabled ? "removeItem" : "removeWorkshopItem"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_delete.svg"
            enabled: contextMenu.publishedFileID === 0
            onClicked: {
                deleteDialog.open()
            }
        }

        MenuItem {
            text: qsTr("Open Workshop Page")
            enabled: contextMenu.publishedFileID !== 0
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_steam.svg"
            onClicked: {
                Qt.openUrlExternally(
                            "steam://url/CommunityFilePage/" + contextMenu.publishedFileID)
            }
        }
    }
    Labs.FileDialog {
        id: exportFileDialog
        fileMode: FileDialog.SaveFile
        property string absoluteStoragePath
        onAccepted: {
            const success = App.util.exportProject(
                              exportFileDialog.absoluteStoragePath,
                              exportFileDialog.currentFile)
        }

        Dialog {
            id: exportFileProgressDialog
            modal: true
            anchors.centerIn: Overlay.overlay
            standardButtons: Dialog.Ok
            onAccepted: errorDialog.close()

            ProgressBar {
                id: exportFileProgressBar
                anchors.centerIn: parent
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
            App.installedListModel.deinstallItemAt(
                        contextMenu.absoluteStoragePath)
        }
    }

    InstalledNavigation {
        id: navWrapper

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        property string filePath
        onEntered: function (drag) {
            dropPopup.open()
        }
        onDropped: function (drop) {
            dropPopup.close()
            dropArea.enabled = false

            if (drop.urls.length > 1) {
                errorDialog.title = qsTr(
                            "We only support adding one item at once.")
                errorDialog.open()
                return
            }
            var file = "" // Convert url to string
            file = "" + drop.urls[0]
            if (!file.endsWith('.screenplay')) {
                errorDialog.title = qsTr(
                            "File type not supported. We only support '.screenplay' files.")
                errorDialog.open()
                return
            }
            importDialog.open()
            dropArea.filePath = file
        }
        onExited: {
            dropPopup.close()
        }

        Dialog {
            id: errorDialog
            modal: true
            anchors.centerIn: Overlay.overlay
            standardButtons: Dialog.Ok
            onAccepted: errorDialog.close()
        }
        Dialog {
            id: importDialog
            modal: true
            anchors.centerIn: Overlay.overlay
            standardButtons: Dialog.Ok
            RowLayout {
                Text {
                    text: qsTr("Import Content...")
                }
            }
            onOpened: {
                App.util.importProject(dropArea.filePath,
                                       App.globalVariables.localStoragePath)
                dropArea.filePath = ""
            }
            onAccepted: {
                importDialog.close()
            }
        }
    }

    Popup {
        id: dropPopup
        anchors.centerIn: Overlay.overlay
        width: root.width * .95
        height: root.height * .95
        dim: true
        modal: true
        onOpened: fileDropAnimation.state = "fileDrop"
        onClosed: {
            fileDropAnimation.state = ""
            dropArea.enabled = true
        }

        Util.FileDropAnimation {
            id: fileDropAnimation
            anchors.centerIn: parent
        }
    }
}
