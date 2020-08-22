import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import ScreenPlay 1.0

Rectangle {
    id: root
    color: Material.theme === Material.Light ? Material.background : Qt.darker(
                                                   Material.background)

    height: availableHeight
    width: parent.width
    clip: true
    layer.enabled: true
    layer.effect: InnerShadow {
        cached: true
        fast: true
        smooth: true
        radius: 32
        spread: .8
        verticalOffset: 3
        color: "#55000000"
    }
    // Width of the Sidebar or Space that should be used
    property real availableWidth: 0
    property real availableHeight: 0
    property int fontSize: 12

    property bool monitorWithoutContentSelectable: true
    property bool multipleMonitorsSelectable: false

    // We preselect the main monitor
    property var activeMonitors: [0]

    property alias background: root.color
    property alias radius: root.radius

    signal requestProjectSettings(int index, var installedType, string appID)

    Component.onCompleted: {
        resize()
    }

    Connections {
        target: ScreenPlay.monitorListModel
        function onMonitorReloadCompleted() {
            resize()
        }
    }

    function deselectOthers(index) {
        for (var i = 0; i < rp.count; i++) {

            if (i === index) {
                rp.itemAt(i).isSelected = true
                continue
            }
            rp.itemAt(i).isSelected = false
        }
    }

    function reset() {
        for (var i = 0; i < rp.count; i++) {
            rp.itemAt(i).isSelected = false
        }
        rp.itemAt(0).isSelected = true
        getActiveMonitors()
    }

    function getActiveMonitors() {
        root.activeMonitors = []
        for (var i = 0; i < rp.count; i++) {
            if (rp.itemAt(i).isSelected) {
                root.activeMonitors.push(rp.itemAt(i).index)
            }
        }
        // Must be called manually. When QML properties are getting altered in js the
        // property binding breaks
        root.activeMonitorsChanged()
        return root.activeMonitors
    }

    function resize() {

        var absoluteDesktopSize = ScreenPlay.monitorListModel.getAbsoluteDesktopSize()
        var isWidthGreaterThanHeight = false
        var windowsDelta = 0

        if (absoluteDesktopSize.width < absoluteDesktopSize.height) {
            windowsDelta = absoluteDesktopSize.width / absoluteDesktopSize.height
            isWidthGreaterThanHeight = false
        } else {
            windowsDelta = absoluteDesktopSize.height / absoluteDesktopSize.width
            isWidthGreaterThanHeight = true
        }

        if (rp.count === 1) {
            availableWidth = availableWidth * .66
        }

        var dynamicHeight = availableWidth * windowsDelta
        var dynamicWidth = availableHeight * windowsDelta

        // Delta (height/width)
        var monitorHeightRationDelta = 0
        var monitorWidthRationDelta = 0

        if (isWidthGreaterThanHeight) {
            monitorHeightRationDelta = dynamicHeight / absoluteDesktopSize.height
            monitorWidthRationDelta = availableWidth / absoluteDesktopSize.width
        } else {
            monitorHeightRationDelta = availableHeight / absoluteDesktopSize.height
            monitorWidthRationDelta = dynamicWidth / absoluteDesktopSize.width
        }

        for (var i = 0; i < rp.count; i++) {
            rp.itemAt(i).index = i
            rp.itemAt(i).height = rp.itemAt(i).height * monitorHeightRationDelta
            rp.itemAt(i).width = rp.itemAt(i).width * monitorWidthRationDelta
            rp.itemAt(i).x = rp.itemAt(i).x * monitorWidthRationDelta
            rp.itemAt(i).y = rp.itemAt(i).y * monitorHeightRationDelta

            rp.contentWidth += rp.itemAt(i).width
            rp.contentHeight += rp.itemAt(i).height
        }
        rp.contentWidth += 200
        rp.contentHeight += 200
    }

    Flickable {
        id: flickable
        anchors.fill: parent

        contentWidth: rp.contentWidth
        contentHeight: rp.contentHeight
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOff

            snapMode: ScrollBar.SnapOnRelease
        }
        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOff

            snapMode: ScrollBar.SnapOnRelease
        }

        Repeater {
            id: rp
            model: ScreenPlay.monitorListModel
            property int contentWidth
            property int contentHeight

            Component.onCompleted: rp.itemAt(0).isSelected = true

            delegate: MonitorSelectionItem {
                id: delegate
                monitorID: m_monitorID
                monitorName: m_name
                appID: m_appID
                height: m_availableGeometry.height
                width: m_availableGeometry.width
                x: m_availableGeometry.x
                y: m_availableGeometry.y
                monitorManufacturer: m_manufacturer
                monitorModel: m_model
                monitorSize: m_availableGeometry
                fontSize: root.fontSize
                index: m_number
                previewImage: m_previewImage
                installedType: m_installedType
                monitorWithoutContentSelectable: root.monitorWithoutContentSelectable

                onMonitorSelected: {

                    if (!multipleMonitorsSelectable) {
                        deselectOthers(index)
                    } else {
                        rp.itemAt(index).isSelected = !rp.itemAt(
                                    index).isSelected
                    }

                    getActiveMonitors()

                    if (delegate.hasContent)
                        root.requestProjectSettings(delegate.index,
                                                    delegate.installedType,
                                                    delegate.appID)
                }
            }
        }
    }
}
