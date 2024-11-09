import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import QtCore as QCore
import ScreenPlayApp
import ScreenPlayCore as Util
import "../Components"

Item {
    id: root

    property bool refresh: false
    property bool enabled: true

    property Item modalSource
    signal setNavigationItem(var pos)

    function checkIsContentInstalled() {
        if (App.installedListModel.count === 0) {
            gridView.footerItem.isVisible = true;
            gridView.visible = false;
            navWrapper.visible = false;
        } else {
            gridView.footerItem.isVisible = false;
            refresh = false;
            gridView.contentY = -82;
            gridView.visible = true;
            navWrapper.visible = true;
        }
    }

    InstalledDrawer {
        id: installedDrawer
        objectName: "installedDrawer"
        width: root.width
    }

    StackView.onActivated: {
        navWrapper.state = "in";
        checkIsContentInstalled();
    }

    Action {
        shortcut: "F5"
        onTriggered: App.installedListModel.reset()
    }

    Connections {
        function onInstalledLoadingFinished() {
            checkIsContentInstalled();
        }

        function onCountChanged(count) {
            if (count === 0)
                checkIsContentInstalled();
        }

        target: App.installedListModel
    }

    ScreenPlayProPopup {
        id: screenPlayProView
    }

    Connections {
        function onSortChanged() {
            gridView.positionViewAtBeginning();
        }

        target: App.installedListFilter
    }

    GridView {
        id: gridView
        objectName: "gridView"

        property bool isDragging: false
        property bool isScrolling: gridView.verticalVelocity !== 0

        anchors.fill: parent
        cellWidth: 340
        cellHeight: 200
        cacheBuffer: 160
        interactive: root.enabled
        // flickDeceleration: 0.00001
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
                gridView.headerItem.isVisible = true;
            else
                gridView.headerItem.isVisible = false;
            //Pull to refresh
            if (contentY <= -180 && !refresh && !isDragging)
                App.installedListModel.reset();
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

        delegate: InstalledItem {
            id: delegate
            isScrolling: gridView.isScrolling
            onClicked: function (folderName, type) {
                installedDrawer.setInstalledDrawerItem(folderName, type);
            }

            onOpenOpenLicensePopup: function () {
                screenPlayProView.open();
            }

            onOpenContextMenu: function (position) {
                // Set the menu to the current item informations
                contextMenu.publishedFileID = delegate.publishedFileID;
                contextMenu.absoluteStoragePath = delegate.absoluteStoragePath;
                contextMenu.fileName = delegate.title;
                contextMenu.type = delegate.type;
                print(delegate.publishedFileID);
                if (contextMenu.godotItem)
                    contextMenu.godotItem.destroy();
                const pos = delegate.mapToItem(root, position.x, position.y);
                // Disable duplicate opening. The can happen if we
                // call popup when we are in the closing animtion.
                if (contextMenu.visible || contextMenu.opened)
                    return;
                if (delegate.type === Util.ContentTypes.InstalledType.GodotWallpaper) {
                    contextMenu.godotItem = editGodotWallpaperComp.createObject();
                    contextMenu.insertItem(0, contextMenu.godotItem);
                }
                contextMenu.popup(pos.x, pos.y);
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }
    Component {
        id: editGodotWallpaperComp

        MenuItem {
            text: qsTr("Edit Wallpaper")
            objectName: "editWallpaper"
            enabled: contextMenu.type === Util.ContentTypes.InstalledType.GodotWallpaper
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_edit.svg"
            onClicked: {
                App.util.openGodotEditor(contextMenu.absoluteStoragePath, App.globalVariables.godotEditorExecutablePath);
            }
        }
    }
    Menu {
        id: contextMenu
        objectName: "installedItemContextMenu"
        // Must be var to support 64-bit size!
        property var publishedFileID: 0
        property var type: 0
        property url absoluteStoragePath
        property string fileName
        // We need to dynamically add this menu item
        // if it is a Godot Wallpaper, see onOpenContextMenu
        property var godotItem

        MenuItem {
            text: qsTr("Open containing folder")
            objectName: "openFolder"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_folder_open.svg"
            onClicked: {
                App.util.openFolderInExplorer(contextMenu.absoluteStoragePath);
            }
        }

        MenuItem {
            text: qsTr("Export")
            objectName: enabled ? "removeItem" : "removeWorkshopItem"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_import_export_.svg"
            onClicked: {
                exportFileDialog.absoluteStoragePath = contextMenu.absoluteStoragePath;
                let urlFileName = QCore.StandardPaths.writableLocation(QCore.StandardPaths.DesktopLocation) + "/" + contextMenu.fileName + ".screenplay";
                exportFileDialog.currentFile = urlFileName;
                exportFileDialog.open();
            }
        }

        MenuItem {
            text: enabled ? qsTr("Remove Item") : qsTr("Remove via Workshop")
            objectName: enabled ? "removeItem" : "removeWorkshopItem"
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_delete.svg"
            enabled: contextMenu.publishedFileID === 0 || !App.globalVariables.isSteamVersion()
            onClicked: {
                deleteDialog.open();
            }
        }

        MenuItem {
            text: qsTr("Open Workshop Page")
            enabled: contextMenu.publishedFileID !== 0 && App.globalVariables.isSteamVersion()
            icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_steam.svg"
            onClicked: {
                Qt.openUrlExternally("steam://url/CommunityFilePage/" + contextMenu.publishedFileID);
            }
        }
    }
    Util.Dialog {
        id: deleteDialog
        title: qsTr("Are you sure you want to delete this item?")
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true
        dim: true
        modalSource: root.modalSource
        anchors.centerIn: Overlay.overlay
        onAccepted: {
            installedDrawer.close();
            if (!App.installedListModel.deinstallItemAt(contextMenu.absoluteStoragePath)) {
                console.error("Unable to uninstall item", contextMenu.absoluteStoragePath);
            }
        }
    }

    FileDialog {
        id: exportFileDialog
        fileMode: FileDialog.SaveFile
        property string absoluteStoragePath
        onAccepted: {
            exportFileProgressDialog.open();
        }
    }

    Util.Dialog {
        id: exportFileProgressDialog
        modal: true
        anchors.centerIn: Overlay.overlay
        width: 400
        focus: true
        modalSource: root.modalSource
        closePolicy: Popup.NoAutoClose
        onOpened: {
            const success = archive.exportProject(exportFileDialog.absoluteStoragePath, exportFileDialog.currentFile);
        }

        onClosed: exportProgressBar.value = 0
        ColumnLayout {
            width: parent.width
            spacing: 20

            Text {
                text: qsTr("Export Content...")
                color: Material.primaryTextColor
                font.pointSize: 18
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            ProgressBar {
                id: exportProgressBar
                from: 0
                to: 100
                Layout.alignment: Qt.AlignHCenter
            }
        }
        Connections {
            id: exportConnections
            target: archive
            function onCompressionProgressChanged(file, proc, total, br, bt) {
                exportProgressBar.value = (br * 100 / bt);
            }
            function onCompressionFinished() {
                exportFileProgressDialog.close();
            }
        }
    }

    InstalledNavigation {
        id: navWrapper
        installedDrawer: installedDrawer

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }

    Util.Archive {
        id: archive
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        property string filePath
        onEntered: function (drag) {
            dropPopup.open();
        }
        onDropped: function (drop) {
            dropPopup.close();
            dropArea.enabled = false;
            if (drop.urls.length > 1) {
                importProjectErrorDialog.title = qsTr("We only support adding one item at once.");
                importProjectErrorDialog.open();
                return;
            }
            var file = "";
            // Convert url to string
            file = "" + drop.urls[0];
            if (!file.endsWith('.screenplay')) {
                importProjectErrorDialog.title = qsTr("File type not supported. We only support '.screenplay' files.");
                importProjectErrorDialog.open();
                return;
            }
            importDialog.open();
            dropArea.filePath = file;
        }
        onExited: {
            dropPopup.close();
        }

        Util.Dialog {
            id: importProjectErrorDialog
            modal: true
            modalSource: root.modalSource
            anchors.centerIn: Overlay.overlay
            standardButtons: Dialog.Ok
            onAccepted: importProjectErrorDialog.close()
        }
        Util.Dialog {
            id: importDialog
            modal: true
            modalSource: root.modalSource
            anchors.centerIn: Overlay.overlay
            width: 400
            focus: true
            closePolicy: Popup.NoAutoClose
            onClosed: importProgressBar.value = 0
            onOpened: {
                const success = archive.importProject(dropArea.filePath, App.globalVariables.localStoragePath);
                print("finished", success);
                dropArea.filePath = "";
            }
            ColumnLayout {
                width: parent.width
                spacing: 20
                Text {
                    text: qsTr("Import Content...")
                    color: Material.primaryTextColor
                    font.pointSize: 18
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
                ProgressBar {
                    id: importProgressBar
                    from: 0
                    to: 100
                    Layout.alignment: Qt.AlignHCenter
                }
                Connections {
                    id: importConnections
                    target: archive
                    function onExtractionProgressChanged(file, proc, total, br, bt) {
                        importProgressBar.value = (br * 100 / bt);
                    }
                    function onExtractionFinished() {
                        importDialog.close();
                    }
                }
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
            fileDropAnimation.state = "";
            dropArea.enabled = true;
        }

        Util.FileDropAnimation {
            id: fileDropAnimation
            anchors.centerIn: parent
        }
    }
}
