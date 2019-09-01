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

    // We preselect the main monitor
    property var activeMonitors:[0]

    property alias background: rect.color
    property alias radius: rect.radius

    signal requestProjectSettings(var at)

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


    Component.onCompleted: {
        resize()
    }

    Connections {
        target: monitorListModel
        onMonitorReloadCompleted: {
            resize()
        }
        onSetNewActiveMonitor: {
            rp.itemAt(index).previewImage = "file:///" + path.trim()
            rp.itemAt(index).isSelected = true
        }
    }
    Connections {
        target: screenPlay
        onAllWallpaperRemoved: {
            for (var i = 0; i < rp.count; i++) {
                rp.itemAt(i).isSelected = false
                rp.itemAt(i).previewImage = ""
            }
        }
    }

    function resize() {
        //  Absolute availableVirtualGeometry
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
            //print(rp.itemAt(i).index,rp.itemAt(i).height)
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
        anchors.centerIn: parent
        model: monitorListModel

        Component.onCompleted: rp.itemAt(0).isSelected = true

        delegate: MonitorSelectionItem {
            id: delegate
            height: monitorAvailableGeometry.height
            width: monitorAvailableGeometry.width
            x: monitorAvailableGeometry.x
            y: monitorAvailableGeometry.y
            monitorManufacturer: monitorManufacturer
            monitorName: monitorName
            monitorModel: monitorModel
            monitorSize: monitorAvailableGeometry
            monitorID: monitorID
            fontSize: rect.fontSize
            index: monitorNumber
            //isWallpaperActive: monitorIsWallpaperActive
            previewImage: monitorPreviewImage

            onMonitorSelected: {
                getActiveMonitors()

                requestProjectSettings(index)
            }
        }
    }
}
