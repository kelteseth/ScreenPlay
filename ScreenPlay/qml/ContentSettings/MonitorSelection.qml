import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import ScreenPlayApp

Rectangle {
    id: root

    property real availableWidth: 0
    property real availableHeight: 0
    property int fontSize: 12
    property bool monitorWithoutContentSelectable: true
    property bool multipleMonitorsSelectable: false
    property bool isSelected: false
    property var activeMonitors: []
    property alias background: root.color
    property alias bgRadius: root.radius

    Component.onCompleted: {
        resize();
        selectOnly(0);
    }
    LoggingCategory {
        id: logger
        name: "MonitorSelection"
        defaultLogLevel: LoggingCategory.Debug
    }

    signal requestProjectSettings(var index, var installedType, var appID)

    function selectOnly(index) {
        for (var i = 0; i < rp.count; i++) {
            if (i === index) {
                rp.itemAt(i).isSelected = true;
                continue;
            }
            rp.itemAt(i).isSelected = false;
        }
        getActiveMonitors();
    }

    function reset() {
        for (var i = 0; i < rp.count; i++) {
            rp.itemAt(i).isSelected = false;
        }
        rp.itemAt(0).isSelected = true;
        getActiveMonitors();
    }

    function getActiveMonitors() {
        root.activeMonitors = [];
        for (var i = 0; i < rp.count; i++) {
            if (rp.itemAt(i).isSelected)
                root.activeMonitors.push(rp.itemAt(i).index);
        }
        root.activeMonitorsChanged();
        root.isSelected = root.activeMonitors.length > 0;
        return root.activeMonitors;
    }

    function selectMonitorAt(index) {
        if (!multipleMonitorsSelectable)
            selectOnly(index);
        else
            rp.itemAt(index).isSelected = !rp.itemAt(index).isSelected;
        getActiveMonitors();
        if (rp.itemAt(index).hasContent)
            root.requestProjectSettings(index, rp.itemAt(index).installedType, rp.itemAt(index).appID);
    }

    function resize() {
        console.debug(logger, "MonitorSelection resize started");

        // 1. Get the total desktop size
        let totalDesktopSize = App.monitorListModel.totalDesktopSize();
        console.debug(logger, "Total desktop size:", totalDesktopSize.width, "x", totalDesktopSize.height);

        // 2. Get root item dimensions
        let rootWidth = root.width;
        let rootHeight = root.height;
        console.debug(logger, "Root dimensions:", rootWidth, "x", rootHeight);

        // 3. Calculate scaling factor
        let margin = 10;
        let availableWidth = rootWidth - 2 * margin;
        let availableHeight = rootHeight - 2 * margin;
        let scaleX = availableWidth / totalDesktopSize.width;
        let scaleY = availableHeight / totalDesktopSize.height;
        let scaleFactor = Math.min(scaleX, scaleY, 1);

        // Ensure we don't scale up
        console.debug(logger, "Scale factor:", scaleFactor);

        // 4. Resize and position repeater items
        let scaledWidth = totalDesktopSize.width * scaleFactor;
        let scaledHeight = totalDesktopSize.height * scaleFactor;
        for (var i = 0; i < rp.count; i++) {
            let item = rp.itemAt(i);
            if (item) {
                item.width = item.geometry.width * scaleFactor;
                item.height = item.geometry.height * scaleFactor;
                item.x = item.geometry.x * scaleFactor;
                item.y = item.geometry.y * scaleFactor;
            }
        }

        // 6. Center content within Flickable
        flickable.contentWidth = scaledWidth;
        flickable.contentHeight = scaledHeight;
        console.debug(logger, "MonitorSelection resize completed", flickable.contentWidth, flickable.contentHeight);
    }

    color: Material.theme === Material.Light ? Material.background : Qt.darker(Material.background)
    height: availableHeight
    width: parent.width
    clip: true
    layer.enabled: true

    Connections {
        function onMonitorReloadCompleted() {
            resize();
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

                appID: m_appID
                geometry: m_geometry
                fontSize: root.fontSize
                index: m_index
                previewImage: m_previewImage
                installedType: m_installedType
                monitorWithoutContentSelectable: root.monitorWithoutContentSelectable
                onMonitorSelected: function (index) {
                    root.selectMonitorAt(index);
                }
                // onRemoveWallpaper: function(index) {

                // }
            }
        }
    }

    ToolButton {
        onClicked: App.monitorListModel.reset()
        icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/font-awsome/rotate-right-solid.svg"
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
