import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay

Rectangle {
    id: root

    property real availableWidth: 0
    property real availableHeight: 0
    property int fontSize: 12
    property bool monitorWithoutContentSelectable: true
    property bool multipleMonitorsSelectable: false
    property bool isSelected: false
    property bool timelineSwitching: false
    // Do not access it directly until
    // QTBUG-127633 is fixed!
    property list<int> activeMonitors: []
    property alias background: root.color
    property alias bgRadius: root.radius

    Component.onCompleted: {
        resize()
        selectOnly(0)
    }

    signal selected(int index)
    signal requestProjectSettings(int index, var installedType, string appID)
    signal requestRemoveWallpaper(int index)
    signal deselected

    function getActiveMonitors(): list<int> {
        console.debug(LoggingCategories.monitorSelection, "Active monitors:", root.activeMonitors)
        return root.activeMonitors
    }

    function selectOnly(index: int): void {
        console.debug(LoggingCategories.monitorSelection, "selectOnly:", index)
        for (var i = 0; i < rp.count; i++) {
            let monitorSelectionItem = rp.itemAt(i) as MonitorSelectionItem
            if (i === index) {
                monitorSelectionItem.isSelected = true
                continue
            }
            monitorSelectionItem.isSelected = false
        }
        updateActiveMonitors()
        root.selected(index)
    }

    function getSelectedMonitorIndex(): int {
        for (var i = 0; i < rp.count; i++) {
            let monitorSelectionItem = rp.itemAt(i) as MonitorSelectionItem
            if (monitorSelectionItem.isSelected)
                return i
        }
        return -1
    }

    function reset(): void {
        console.debug(LoggingCategories.monitorSelection, "MonitorSelection reset")
        for (var i = 0; i < rp.count; i++) {
            rp.itemAt(i).isSelected = false
        }
        rp.itemAt(0).isSelected = true
        updateActiveMonitors()
    }

    function updateActiveMonitors(): void {
        root.activeMonitors = []
        for (var i = 0; i < rp.count; i++) {
            let monitorSelectionItem = rp.itemAt(i) as MonitorSelectionItem
            if (monitorSelectionItem.isSelected)
                root.activeMonitors.push(monitorSelectionItem.monitorIndex)
        }
        root.isSelected = root.activeMonitors.length > 0
    }

    function selectMonitorAt(index: int): void {
        let monitorSelectionItem = rp.itemAt(index) as MonitorSelectionItem
        if (!multipleMonitorsSelectable) {
            selectOnly(index)
        } else {
            rp.itemAt(index).isSelected = !monitorSelectionItem.isSelected
        }
        updateActiveMonitors()
        if (monitorSelectionItem.hasContent) {
            root.requestProjectSettings(index, monitorSelectionItem.installedType, monitorSelectionItem.appID)
        } else {
            root.deselected()
        }
    }

    function resize(): void {
        console.debug(LoggingCategories.monitorSelection, "MonitorSelection resize started");

        // 1. Get the total desktop size
        let totalDesktopSize = App.monitorListModel.totalDesktopSize()
        console.debug(LoggingCategories.monitorSelection, "Total desktop size:", totalDesktopSize.width, "x", totalDesktopSize.height);

        // 2. Get root item dimensions
        let rootWidth = root.width
        let rootHeight = root.height
        console.debug(LoggingCategories.monitorSelection, "Root dimensions:", rootWidth, "x", rootHeight);

        // 3. Calculate scaling factor
        let margin = 10
        let availableWidth = rootWidth - 2 * margin
        let availableHeight = rootHeight - 2 * margin
        let scaleX = availableWidth / totalDesktopSize.width
        let scaleY = availableHeight / totalDesktopSize.height
        let scaleFactor = Math.min(scaleX, scaleY, 1);

        // Ensure we don't scale up
        console.debug(LoggingCategories.monitorSelection, "Scale factor:", scaleFactor);

        // 4. Resize and position repeater items
        let scaledWidth = totalDesktopSize.width * scaleFactor
        let scaledHeight = totalDesktopSize.height * scaleFactor
        for (var i = 0; i < rp.count; i++) {
            let monitorSelectionItem = rp.itemAt(i) as MonitorSelectionItem
            monitorSelectionItem.width = monitorSelectionItem.geometry.width * scaleFactor
            monitorSelectionItem.height = monitorSelectionItem.geometry.height * scaleFactor
            monitorSelectionItem.x = monitorSelectionItem.geometry.x * scaleFactor
            monitorSelectionItem.y = monitorSelectionItem.geometry.y * scaleFactor
        }

        // 6. Center content within Flickable
        flickable.contentWidth = scaledWidth
        flickable.contentHeight = scaledHeight
        console.debug(LoggingCategories.monitorSelection, "MonitorSelection resize completed", flickable.contentWidth, flickable.contentHeight)
    }

    color: Material.theme === Material.Light ? Material.backgroundColor : Qt.darker(Material.backgroundColor)
    height: availableHeight
    width: parent.width
    clip: true
    layer.enabled: true

    Connections {
        function onMonitorReloadCompleted(): void {
            console.debug(LoggingCategories.monitorSelection, "MonitorSelection: onMonitorReloadCompleted")

            let currentSelectedIndex = root.getSelectedMonitorIndex()
            root.resize();

            // Restore selection or default to 0 if nothing selected
            if (currentSelectedIndex < 0) {
                currentSelectedIndex = 0
            }

            root.selectOnly(currentSelectedIndex)
        }

        target: App.monitorListModel
    }

    Flickable {
        id: flickable

        anchors.fill: parent
        contentWidth: rp.contentWidth
        contentHeight: rp.contentHeight

        Repeater {
            id: rp

            property int contentWidth
            property int contentHeight

            model: App.monitorListModel

            delegate: MonitorSelectionItem {
                id: delegate
                enabled: root.enabled
                fontSize: root.fontSize
                monitorWithoutContentSelectable: root.monitorWithoutContentSelectable
                onMonitorSelected: function (index) {
                    root.selectMonitorAt(index)
                }
                onRemoveWallpaper: function (index) {
                    root.requestRemoveWallpaper(index)
                }
            }
        }
    }

    ToolButton {
        onClicked: App.monitorListModel.reset()
        icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/font-awsome/rotate-right-solid.svg"
        icon.width: 14
        icon.height: 14
        opacity: hovered ? 1 : .4
        hoverEnabled: true
        Behavior on opacity {
            NumberAnimation {
                duration: 250
            }
        }

        anchors {
            top: parent.top
            right: parent.right
        }
    }
}
