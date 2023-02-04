import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlay

Rectangle {
    id: root

    property real availableWidth: 0
    property real availableHeight: 0
    property int fontSize: 12
    property bool monitorWithoutContentSelectable: true
    property bool multipleMonitorsSelectable: false
    property bool isSelected: false
    // We preselect the main monitor
    property var activeMonitors: []
    property alias background: root.color
    property alias radius: root.radius

    Component.onCompleted: {
        resize();
        selectOnly(0);
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
        // Must be called manually. When QML properties are getting altered in js the
        // property binding breaks
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
        var absoluteDesktopSize = App.monitorListModel.absoluteDesktopSize();
        var isWidthGreaterThanHeight = false;
        var windowsDelta = 0;
        if (absoluteDesktopSize.width < absoluteDesktopSize.height) {
            windowsDelta = absoluteDesktopSize.width / absoluteDesktopSize.height;
            isWidthGreaterThanHeight = false;
        } else {
            windowsDelta = absoluteDesktopSize.height / absoluteDesktopSize.width;
            isWidthGreaterThanHeight = true;
        }
        if (rp.count === 1)
            availableWidth = availableWidth * 0.66;
        var dynamicHeight = availableWidth * windowsDelta;
        var dynamicWidth = availableHeight * windowsDelta;
        // Delta (height/width)
        var monitorHeightRationDelta = 0;
        var monitorWidthRationDelta = 0;
        if (isWidthGreaterThanHeight) {
            monitorHeightRationDelta = dynamicHeight / absoluteDesktopSize.height;
            monitorWidthRationDelta = availableWidth / absoluteDesktopSize.width;
        } else {
            monitorHeightRationDelta = availableHeight / absoluteDesktopSize.height;
            monitorWidthRationDelta = dynamicWidth / absoluteDesktopSize.width;
        }
        for (var i = 0; i < rp.count; i++) {
            rp.itemAt(i).index = i;
            rp.itemAt(i).height = rp.itemAt(i).height * monitorHeightRationDelta;
            rp.itemAt(i).width = rp.itemAt(i).width * monitorWidthRationDelta;
            rp.itemAt(i).x = rp.itemAt(i).x * monitorWidthRationDelta;
            rp.itemAt(i).y = rp.itemAt(i).y * monitorHeightRationDelta;
            rp.contentWidth += rp.itemAt(i).width;
            rp.contentHeight += rp.itemAt(i).height;
        }
        rp.contentWidth += 200;
        rp.contentHeight += 200;
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
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOff
            snapMode: ScrollBar.SnapOnRelease
        }

        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOff
            snapMode: ScrollBar.SnapOnRelease
        }
    }
}
