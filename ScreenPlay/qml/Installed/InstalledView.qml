import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Effects
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
    property bool installedLoadingFinished: false

    property Item modalSource
    signal setNavigationItem(var pos)

    function checkIsContentInstalled() {
        if (App.installedListModel.count !== 0) {
            gridView.contentY = -82;
        }
    }
    StackView.onStatusChanged: {
        if (StackView.status == StackView.Deactivating) {
            installedDrawer.close();
        }
        if (StackView.status == StackView.Active) {
            App.installedListModel.reset();
        }
    }

    InstalledDrawer {
        id: installedDrawer
        objectName: "installedDrawer"
        width: root.width
    }

    StackView.onActivated: {
        navWrapper.state = "in";
        root.checkIsContentInstalled();
    }

    Action {
        shortcut: "F5"
        onTriggered: {
            root.installedLoadingFinished = false;
            App.installedListModel.reset();
        }
    }

    Connections {
        function onInstalledLoadingFinished() {
            root.installedLoadingFinished = true;
            root.checkIsContentInstalled();
        }

        function onCountChanged(count) {
            if (count === 0)
                root.checkIsContentInstalled();
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
        visible: root.installedLoadingFinished

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
            if (contentY <= -180 && root.installedLoadingFinished && !isDragging) {
                root.installedLoadingFinished = false;
                App.installedListModel.reset();
            }
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

        // To not cover the items on the botton when the
        // Drawer is active
        footer: Item {
            height: 300
            visible: root.installedLoadingFinished && App.installedListModel.count !== 0
            width: parent.width

            Text {
                id: txtFooter
                font.family: App.settings.font
                font.pointSize: 14
                text: qsTr("Get more Wallpaper & Widgets from the Steam Workshop!")
                color: Material.hintTextColor
                visible: App.globalVariables.isSteamVersion()
                anchors {
                    top: parent.top
                    topMargin: 20
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }

        delegate: InstalledItem {
            id: delegate
            // objectName: "installedItem" + index
            isScrolling: gridView.isScrolling
            focus: true
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


    Pane {
        id: invalidStoragePathView
        anchors.centerIn: parent
        visible: !isValid && root.installedLoadingFinished
        enabled: visible
        property bool isValid: (App.globalVariables.localStoragePath + "") !== ""

        contentItem: ColumnLayout {
            anchors.centerIn: parent
            width: parent.width
            spacing: 16

            Label {
                Layout.fillWidth: true
                Layout.bottomMargin: 8
                horizontalAlignment: Text.AlignHCenter
                color: Material.accentColor
                font.pixelSize: 18
                text: qsTr("⚠️ Storage Path Issue")
                wrapMode: Text.WordWrap
            }

            Label {
                Layout.fillWidth: true
                color: Material.secondaryTextColor
                text: App.globalVariables.isStandaloneVersion() ? qsTr("Unable to access content storage location") : qsTr("Unable to access Steam Workshop folder")
                wrapMode: Text.WordWrap
            }

            Label {
                Layout.fillWidth: true
                Layout.topMargin: 16
                color: Material.secondaryTextColor
                text: {
                   if( App.globalVariables.isStandaloneVersion()) {
                       qsTr("This could be due to:\n• Missing write permissions\n• Antivirus blocking access\n• Storage path no longer exists\n\nTry:\n• Running ScreenPlay as administrator\n• Checking your antivirus settings\n• Reinstalling ScreenPlay")
                   } else {
                       qsTr("This could be due to:\n• Steam is not installed\n• Steam Workshop folder is inaccessible\n• Missing write permissions\n\nTry:\n• Starting Steam first\n• Running ScreenPlay through Steam\n• Verifying game files in Steam\n• Reinstalling ScreenPlay through Steam")
                   }
                }

                wrapMode: Text.WordWrap
                lineHeight: 1.2
            }

            RowLayout {
                Layout.topMargin: 20
                Layout.fillWidth: true

                Button {
                    text: qsTr("Set content folder path")
                    onClicked: App.util.setNavigation("Settings")
                    icon {
                        source:  "qrc:/qml/ScreenPlayApp/assets/icons/icon_settings.svg"
                        width: 20
                        height: 20
                    }
                }
                Button {
                    text: qsTr("Get help in the forums")
                    onClicked: Qt.openUrlExternally("https://forum.screen-play.app/")
                    icon {
                        source:  "qrc:/qml/ScreenPlayApp/assets/icons/icon_open_in_new_black.svg"
                        width: 20
                        height: 20
                    }
                }
            }
        }
    }



    Util.RainbowGradient {
        id: rainbowGradient
        visible: false
        running: noInstalledContentView.enabled
        anchors {
            topMargin: parent.height *.666
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
    }
    MultiEffect {
        id: rainbowEffect
        source: rainbowGradient
        anchors.fill: rainbowGradient

        // Mask effect
        maskSpreadAtMax: .5
        maskSpreadAtMin: 1
        maskThresholdMax: 1
        maskThresholdMin: .5
        maskEnabled: true
        maskSource: Image {
            id: maskImage
            source: "qrc:/qml/ScreenPlayApp/assets/images/first_startup_mask.png"
            width: rainbowGradient.width
            height: rainbowGradient.height
            fillMode: Image.Stretch
        }
        NumberAnimation {
            id: breathingAnimation
            running: noInstalledContentView.enabled
            from: 0
            to: Math.PI * 2
            duration: 3000
            loops: Animation.Infinite
        }
        opacity: noInstalledContentView.enabled ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }
    }
    Pane {
        id: noInstalledContentView
        anchors.centerIn: parent
        visible: App.installedListModel.count === 0 && invalidStoragePathView.isValid && root.installedLoadingFinished
        enabled: visible
        padding: 40
        background: Rectangle {
                color: Material.backgroundColor

                Util.RainbowGradient {
                    id: inlineRainbowGradient
                    visible: noInstalledContentView.visible
                    running: visible
                    anchors.fill: parent
                    radius: 4
                    clip: true

                }

                Rectangle {
                    radius: 4
                    color: Material.backgroundColor
                    anchors.fill: parent
                    anchors.margins: 3
                }
            }




        contentItem: RowLayout {
            anchors.centerIn: parent
            width: parent.width
            spacing: 50
                ColumnLayout {
            spacing: 20

            Label {
                text: qsTr("Lets get started")
                color: Material.accentColor
                Layout.fillWidth: true
                Layout.bottomMargin: 8
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 26
                wrapMode: Text.WordWrap
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                color: Material.primaryTextColor
                text: App.globalVariables.isStandaloneVersion() ? qsTr("Your content library is empty at:") : qsTr("No Steam Workshop items installed at:")
                wrapMode: Text.WordWrap
            }
            Label {
                Layout.fillWidth: true
                Layout.topMargin: -5
                horizontalAlignment: Text.AlignHCenter
                color: Material.secondaryTextColor
                text: App.util.toLocal(App.globalVariables.localStoragePath)
                wrapMode: Text.WordWrap
            }

            Label {
                Layout.fillWidth: true
                Layout.topMargin: 20
                color: Material.secondaryTextColor
                text:{
                    if(App.globalVariables.isStandaloneVersion() ){
                        qsTr("To get started:\n• Click the '+' button to add content\n• Import your existing wallpapers\n• Download community wallpapers\n• Create your own wallpaper")

                    } else {
                        qsTr("To get started:\n• Visit the Steam Workshop\n• Subscribe to wallpapers you like\n• Wait for Steam to download them\n• Create and share your own wallpapers")
                    }
                }
                wrapMode: Text.WordWrap
                lineHeight: 1.2
            }

            RowLayout {
                Layout.topMargin: 20
                Layout.fillWidth: true

                Button {
                    text: App.globalVariables.isStandaloneVersion() ? qsTr("Add Content") : qsTr("Open Workshop")
                    onClicked: {
                        if(App.globalVariables.isStandaloneVersion()){
                            App.util.setNavigation("Create")
                        } else {
                            App.util.setNavigation("Workshop")
                        }
                    }

                    icon {
                        source: App.globalVariables.isStandaloneVersion() ? "qrc:/qml/ScreenPlayApp/assets/icons/icon_plus.svg" :"qrc:/qml/ScreenPlayApp/assets/icons/icon_steam.svg"
                        width: 20
                        height: 20
                    }
                }
                Button {
                    text: qsTr("Open Install Folder Path")
                    onClicked: App.util.openFolderInExplorer(App.globalVariables.localStoragePath.toString())
                    icon {
                        source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_folder_open.svg"
                        width: 20
                        height: 20
                    }
                }
            }

            }
                Item {
                         Layout.preferredWidth: ship.width
                         Layout.preferredHeight: ship.height + 20  // Added extra space for the animation
                         Image {
                             id: ship
                             source: "qrc:/qml/ScreenPlayApp/assets/images/rocket_3d.png"

                             SequentialAnimation {
                                 running: true
                                 loops: Animation.Infinite

                                 NumberAnimation {
                                     target: ship
                                     property: "y"
                                     from: 0
                                     to: 10
                                     duration: 1500
                                     easing.type: Easing.InOutQuad
                                 }
                                 NumberAnimation {
                                     target: ship
                                     property: "y"
                                     from: 10
                                     to: 0
                                     duration: 1500
                                     easing.type: Easing.InOutQuad
                                 }
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
    InstalledNavigation {
        id: navWrapper
        installedDrawer: installedDrawer
        visible: root.installedLoadingFinished

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }
}
