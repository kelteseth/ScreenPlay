import QtQuick 2.12
import QtGraphicalEffects 1.0

Rectangle {
    id: rect

    height: availableHeight
    width: availableWidth

    // Width of the Sidebar or Space that should be used
    property real availableWidth: 0
    property real availableHeight: 0
    property int fontSize: 12
    property bool multipleMonitorsSelectable: true

    // We preselect the main monitor
    property var activeMonitors:[0]

    property alias background: rect.color
    property alias radius: rect.radius

    signal requestProjectSettings(var at)

    Component.onCompleted: {
        resize()
    }

    Connections {
        target: monitorListModel
        onMonitorReloadCompleted: {
            resize()
        }
    }

    function deselectOthers(index){
        for (var i = 0; i < rp.count; i++) {

            if(i === index){
                rp.itemAt(i).isSelected = true
                continue;
            }
            rp.itemAt(i).isSelected = false

        }

    }

    function deselectAll(){
        for (var i = 0; i < rp.count; i++) {
            rp.itemAt(i).isSelected = false
        }
        getActiveMonitors()
    }

    function getActiveMonitors(){
        rect.activeMonitors = []
        for (var i = 0; i < rp.count; i++) {
             if(rp.itemAt(i).isSelected){
                 rect.activeMonitors.push(rp.itemAt(i).index)
             }
        }
        // Must be called manually. When QML properties are getting altered in js the
        // property binding breaks
        rect.activeMonitorsChanged()
        return rect.activeMonitors
    }

    function resize() {

        var absoluteDesktopSize = monitorListModel.getAbsoluteDesktopSize()
        var isWidthGreaterThanHeight = false
        var windowsDelta = 0

        if (absoluteDesktopSize.width < absoluteDesktopSize.height) {
            windowsDelta = absoluteDesktopSize.width / absoluteDesktopSize.height
            isWidthGreaterThanHeight = false
        } else {
            windowsDelta = absoluteDesktopSize.height / absoluteDesktopSize.width
            isWidthGreaterThanHeight = true
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

        }
    }

    Repeater {
        id: rp
        anchors.fill: parent
        model: monitorListModel

        Component.onCompleted: rp.itemAt(0).isSelected = true

        delegate: MonitorSelectionItem {
            id: delegate
            monitorID: m_monitorID
            monitorName: m_name
            height: m_availableGeometry.height
            width: m_availableGeometry.width
            x: m_availableGeometry.x
            y: m_availableGeometry.y
            monitorManufacturer: m_manufacturer
            monitorModel: m_model
            monitorSize: m_availableGeometry
            fontSize: rect.fontSize
            index: m_number
            previewImage: m_previewImage

            onMonitorSelected: {

                if(!multipleMonitorsSelectable){
                    deselectOthers(index)
                } else {
                      rp.itemAt(index).isSelected = !rp.itemAt(index).isSelected
                }


                getActiveMonitors()

                requestProjectSettings(index)
            }
        }
    }
}
