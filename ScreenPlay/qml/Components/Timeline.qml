pragma ValueTypeBehavior

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

Control {
    id: root
    // Prefix applied to every objectName inside this Timeline so multiple
    // instances (e.g. drawer + settings) can be addressed by chuck without
    // collisions. Defaults are kept stable for existing call sites.
    property string objectNamePrefix: "timeline"
    objectName: root.objectNamePrefix + "Root"
    height: 160
    implicitWidth: 600
    topPadding: 20
    leftPadding: 20
    rightPadding: 20

    signal ready

    // User selected
    property int selectedTimelineIndex: 0
    // Do not use this directly, see https://bugreports.qt.io/browse/QTBUG-127633
    property var selectedTimeline: timeline.sectionsList[root.selectedTimelineIndex]
    // Bound to the C++ source of truth so it actually updates on add/remove —
    // QML var-array bindings do not observe in-place push/sort.
    readonly property int length: App.screenPlayManager.timelineSectionCount
    readonly property int activeTimelineIndex: App.screenPlayManager.activeTimelineIndex
    property Item modalSource
    // Inverts the default 15-minute snap on handle drags. Toggled by Shift
    // press/release on any LineHandle that currently has active focus (which
    // is the handle the user is dragging); see LineHandle.qml.
    property bool freeDragModifier: false

    function getSelectedTimeline(): TimelineSection {
        return timeline.sectionsList[root.selectedTimelineIndex]
    }

    function setActiveWallpaperPreviewImage(): void {
        timeline.setActiveWallpaperPreviewImage()
    }

    function removeAll(): void {
        timeline.removeAll()
    }

    function reset(): void {
        timeline.reset()
        timeline.load()
        root.ready()
    }

    Connections {
        target: App.screenPlayManager
        function onPrintQmlTimeline(): void {
            console.debug(LoggingCategories.timeline, "################# qml:")
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                console.debug(LoggingCategories.timeline, timeline.sectionsList[i].index, timeline.sectionsList[i].identifier, timeline.sectionsList[i].relativeLinePosition)
            }
        }
    }

    contentItem: Item {
        id: timeline
        height: 160
        implicitWidth: 600

        property var sectionsList: []
        property var lineColors: ["#1E88E5", "#00897B", "#43A047", "#C0CA33", "#FFB300", "#FB8C00", "#F4511E", "#E53935", "#D81B60", "#8E24AA", "#5E35B1", "#3949AB"]

        // Each handle's lineWidth is bound to the track width in
        // createSection, so the startup churn where timeline.width updates
        // several times (implicitWidth 600 → actual 608) just re-evaluates
        // each handle's x binding. No manual per-handle width re-sync — and
        // crucially no frozen creation-time width that desynced drag bounds.
        function load(): void {
            timeline.reset()
            timeline.updatePositions()
            timeline.updateActiveTimelineIndex(App.screenPlayManager.activeTimelineIndex)
        }

        // Component.onCompleted: reset()

        Connections {
            target: App.screenPlayManager
            function onNotifyUiReloadTimelinePreviewImage(): void {
                timeline.setActiveWallpaperPreviewImage()
            }
            function onActiveTimelineIndexChanged(activeTimelineIndex: int): void {
                timeline.updateActiveTimelineIndex(activeTimelineIndex)
            }
        }

        function updateActiveTimelineIndex(activeTimelineIndex: int): bool {
            if (activeTimelineIndex === -1) {
                return false
            }
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let section = timeline.sectionsList[i] as TimelineSection
                const isNewActive = activeTimelineIndex === i
                section.lineIndicator.isActive = isNewActive
                section.lineIndicator.selected = isNewActive
                if (isNewActive)
                    section.lineIndicator.lineSelected(section.lineIndicator.index)
            }
            return true
        }

        function createTimelineAt(relativePosition: real): bool {
            const identifier = App.util.generateRandomString(4)
            const sectionIndex = timeline.addSection(identifier, relativePosition)
            const sectionObject = timeline.sectionsList[sectionIndex]
            const addTimelineAtSuccess = App.screenPlayManager.addTimelineAt(sectionObject.index, sectionObject.relativeLinePosition, sectionObject.identifier)
            if (!addTimelineAtSuccess) {
                // C++ rejected the split (e.g. zero-length section because the click
                // landed in the same second as an existing handle). Roll back the
                // QML-side handle/indicator that addSection already created so we
                // don't leave orphan widgets the C++ model never knew about.
                sectionObject.lineHandle.destroy()
                sectionObject.lineIndicator.destroy()
                sectionObject.destroy()
                timeline.sectionsList.splice(sectionIndex, 1)
                timeline.updatePositions()
                InstantPopup.openErrorPopup(timeline, qsTr("Unable to add Timeline"))
                return false
            }
            return true
        }

        function reset(): void {
            console.warn(LoggingCategories.timeline, "⚠️ RESET")
            removeAll()
            let initialSectionsList = App.screenPlayManager.timelineSections()
            initialSectionsList.sort(function (a, b) {
                return b.index - a.index
            })
            for (let index in initialSectionsList) {
                let section = initialSectionsList[index]
                addSection(section.identifier, section.relativePosition)
            }
            const activeTimelineIndex = App.screenPlayManager.activeTimelineIndex
            if (activeTimelineIndex === -1) {
                return
            }
            lineIndicatorSelected(activeTimelineIndex)
            setActiveWallpaperPreviewImage()
        }

        function setActiveWallpaperPreviewImage(): void {
            if (timeline.sectionsList.length == 0) {
                console.error(LoggingCategories.timeline, "Cannot set preview image with an empty section list")
                return
            }

            let timelineSectionList = App.screenPlayManager.timelineSections()
            for (var i = 0; i < timelineSectionList.length; i++) {
                let timelineSection = timelineSectionList[i]
                if (!timeline.sectionsList[i]) {
                    console.error(LoggingCategories.timeline, "No sectionList item at index:", i, " of ", timeline.sectionsList.length)
                    return
                }

                let lineIndicator = timeline.sectionsList[i].lineIndicator

                // Reset the preview image
                if (timelineSection.wallpaperData.length === 0) {
                    lineIndicator.wallpaperPreviewImage = ""
                    continue
                }

                let firstWallpaper = timelineSection.wallpaperData[0]
                lineIndicator.wallpaperPreviewImage = Qt.resolvedUrl("file:///" + firstWallpaper.absolutePath + "/" + firstWallpaper.previewImage)
            }
        }

        function removeAll(): void {
            console.debug(LoggingCategories.timeline, "removeAll", timeline.sectionsList.length)
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                // ORDER is important here! Destory the children first
                console.debug(LoggingCategories.timeline, "remove index ", i)
                let section = timeline.sectionsList[i]
                section.lineHandle.destroy()
                section.lineIndicator.destroy()
                section.destroy()
            }
            timeline.sectionsList = [];
            // Default to the first timeline
            root.selectedTimelineIndex = 0
        }

        // IMPORTANT: The new element is always on the left. The first
        // handle always persists  because the
        // user can never delete it. It only gets "pushed" further
        // to the right, by decreasing its size.
        function addSection(identifier: string, stopPosition: real): int {
            console.debug(LoggingCategories.timeline, "stopPosition", stopPosition);

            // Make sure to limit float precision
            const fixedStopPosition = stopPosition
            console.debug(LoggingCategories.timeline, "addSection at: ", fixedStopPosition)
            if (stopPosition < 0 || fixedStopPosition > 1) {
                console.error(LoggingCategories.timeline, "Invalid position:", fixedStopPosition)
                return -1
            }
            const sectionComp = Qt.createComponent("TimelineSection.qml")
            if (sectionComp.status === Component.Error) {
                console.assert(LoggingCategories.timeline, sectionComp.errorString())
                return -1
            }
            let sectionObject = sectionComp.createObject(timeline, {
                "identifier": identifier,
                "relativeLinePosition": fixedStopPosition
            })
            timeline.sectionsList.push(sectionObject)
            timeline.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition
            })
            const index = timeline.sectionsList.indexOf(sectionObject)
            console.debug(LoggingCategories.timeline, "Addsection:", index)
            createSection(index, fixedStopPosition, sectionObject, identifier)
            updatePositions()
            return index
        }

        // Disables all other LineHandles if one is in use. Enables
        // all after the user released the LineHandle
        function setActiveHandle(identifier: string, active: bool): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let lineHandle = timeline.sectionsList[i].lineHandle
                if (active) {
                    if (lineHandle.identifier !== identifier) {
                        lineHandle.otherLineHandleActive = true
                    }
                } else {
                    lineHandle.otherLineHandleActive = false
                }
            }
        }

        function createSection(index: int, stopPosition: real, section: TimelineSection, identifier: string): void {
            console.debug(LoggingCategories.timeline, "Adding at:", index, stopPosition, identifier)

            let haComponent = Qt.createComponent("LineHandle.qml")
            if (haComponent.status === Component.Error) {
                console.assert(LoggingCategories.timeline, haComponent.errorString())
                return
            }
            section.lineHandle = haComponent.createObject(handleWrapper)
            // Pixels are derived from the model: bind the handle's lineWidth to
            // the live track width so its x re-evaluates on resize instead of
            // freezing at the creation-time width.
            section.lineHandle.lineWidth = Qt.binding(function () { return handleWrapper.width })
            // endSeconds is the source of truth; LineHandle derives x from it.
            // Rounding to the nearest minute recovers the exact boundary that
            // the 4-decimal-serialized relativePosition rounds away on load,
            // and is a no-op for freshly snapped inserts. Two handles can only
            // share an x if they share a second — which the C++ minSection
            // check already rejects — so the old "collapses to width 0"
            // sub-pixel hazard is gone.
            section.lineHandle.endSeconds = Math.round(stopPosition * 1440) * 60
            // Add vertical offset to make moving easier in the LineHandle.qml
            section.lineHandle.y = 10;
            // minSeconds/maxSeconds are set by updatePositions() right after.
            section.lineHandle.identifier = identifier
            section.lineHandle.handleMoved.connect(timeline.onHandleMoved)
            section.lineHandle.activated.connect(timeline.setActiveHandle);
            // Snap default is on; Shift on the dragged handle flips
            // freeDragModifier and every handle un-snaps in lockstep.
            section.lineHandle.snapEnabled = Qt.binding(function() { return !root.freeDragModifier })
            section.lineHandle.modifierKeyChanged.connect(function(freeDrag) { root.freeDragModifier = freeDrag })
            // Connect the new signal
            let liComponent = Qt.createComponent("LineIndicator.qml")
            if (liComponent.status === Component.Error) {
                console.assert(LoggingCategories.timeline, liComponent.errorString())
                return
            }

            // Set color initially so we do not have a weird color animation at start
            const lineIndicatorProperties = {
                "color": getColorAtIndex(index)
            }
            section.lineIndicator = liComponent.createObject(lineIndicatorWrapper, lineIndicatorProperties)
            section.lineIndicator.height = lineIndicatorWrapper.height
            section.lineIndicator.index = index
            section.lineIndicator.identifier = identifier
            section.lineIndicator.color = getColorAtIndex(index)
            section.lineIndicator.remove.connect(timeline.removeSection)
            section.lineIndicator.lineSelected.connect(timeline.lineIndicatorSelected)
        }

        function sectionFromHandle(lineHandle: LineHandle): TimelineSection {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                if (timeline.sectionsList[i].lineHandle === lineHandle)
                    return timeline.sectionsList[i]
            }
            return null
        }

        function onHandleMoved(lineHandle: LineHandle): void {
            updatePositions()
            const section = sectionFromHandle(lineHandle)
            if (section === null) {
                console.debug(LoggingCategories.timeline, lineHandle.relativePosition)
                console.error(LoggingCategories.timeline, "Unable to match handle to section list")
                return
            }
            // Keep the section's stored position in step with the dragged
            // handle so the next sort/insert sees the live value rather than
            // the creation-time snapshot.
            section.relativeLinePosition = lineHandle.relativePosition
            App.screenPlayManager.moveTimelineAt(section.index, section.identifier, lineHandle.relativePosition, lineHandle.timeString)
        }

        function lineIndicatorSelected(selectedTimelineIndex: int): void {
            console.debug(LoggingCategories.timeline, "selectedTimelineIndex:", selectedTimelineIndex, "section cout: ", timeline.sectionsList.length)
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                const enableTimeline = (i === selectedTimelineIndex)
                timeline.sectionsList[i].lineIndicator.selected = enableTimeline
            }
            root.selectedTimelineIndex = selectedTimelineIndex
            App.screenPlayManager.setSelectedTimelineIndex(selectedTimelineIndex)
        }

        // We must update all indexes when removing/adding an element
        function updateIndicatorIndexes(): void {
            if (timeline.sectionsList === null || timeline.sectionsList === undefined)
                return
            timeline.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition
            })
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                timeline.sectionsList[i].index = i
                timeline.sectionsList[i].lineIndicator.index = i;
                // Keep test-friendly objectNames in sync with current index order.
                timeline.sectionsList[i].lineHandle.objectName = root.objectNamePrefix + "Handle" + i
                timeline.sectionsList[i].lineIndicator.objectName = root.objectNamePrefix + "Indicator" + i
                timeline.sectionsList[i].lineIndicator.objectNamePrefix = root.objectNamePrefix
                //console.debug("updateIndicatorIndexes:", timeline.sectionsList[i].index, timeline.sectionsList[i].relativeLinePosition)
            }
        }

        function removeSection(index: int): void {
            console.debug(LoggingCategories.timeline, "removeSection", index, "sectionsList:", JSON.stringify(timeline.sectionsList.map(s => s.relativeLinePosition)))
            const isLast = index === timeline.sectionsList.length - 1
            if (isLast)
                return
            App.screenPlayManager.removeTimelineAt(index).then(result => {
                if (!result.success) {
                    InstantPopup.openErrorPopup(timeline, result.message)
                    btnReset.resetting = false
                    return
                }
                // Only destroy QML objects after the C++ side succeeds
                let section = timeline.sectionsList[index]
                section.lineHandle.destroy()
                section.lineIndicator.destroy()
                section.destroy()
                timeline.sectionsList.splice(index, 1)
                updatePositions()
                // C++ fires activeTimelineIndexChanged synchronously inside
                // removeTimelineAt — i.e. BEFORE this splice runs — so the
                // signal handler iterates the pre-splice sectionsList and
                // marks the wrong section as active (the one that will land
                // at activeIndex-1 once we splice). Re-apply the active
                // index now that sectionsList matches the C++ ordering.
                updateActiveTimelineIndex(App.screenPlayManager.activeTimelineIndex)
            })
        }

        function updatePositions(): void {
            // minGap is in *seconds* — it IS minSectionSeconds from
            // screenplaytimelinemanager.cpp, no pixel conversion. Offsetting
            // each handle's drag bounds by it stops the user dragging a
            // section below the size the C++ side would reject (which would
            // otherwise leave QML and C++ disagreeing about positions).
            // 300s = 5 minutes; sub-5-min sections render an invalid preview.
            const minGap = 300
            const lastIdx = timeline.sectionsList.length - 1
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let handle = timeline.sectionsList[i].lineHandle

                handle.minSeconds = (i === 0) ? minGap : timeline.sectionsList[i - 1].lineHandle.endSeconds + minGap
                handle.maxSeconds = (i === lastIdx) ? 86400 : timeline.sectionsList[i + 1].lineHandle.endSeconds - minGap

                // Clamp the hit margin to half the distance to the nearest
                // neighbour so hit zones never overlap. Without this, a 30px
                // margin on every handle means a click between two close
                // handles routes to whichever one wins QML hit-testing
                // (typically the later child) rather than the closer one.
                // handle.x is a derived binding now, but it's still the right
                // pixel anchor for a pointer hit-test radius.
                const halfLeft = (i === 0) ? Infinity : (handle.x - timeline.sectionsList[i - 1].lineHandle.x) / 2
                const halfRight = (i === lastIdx) ? Infinity : (timeline.sectionsList[i + 1].lineHandle.x - handle.x) / 2
                const minSide = Math.min(halfLeft, halfRight)
                handle.hitMargin = Math.max(5, Math.min(30, minSide))
            }
            updateIndicatorPositions()
            updateLastHandle()
            updateIndicatorColor()
            updateIndicatorIndexes()
        }

        function getColorAtIndex(index: int): string { // Return hex not color
            let i = index
            // Start from the beginnging again
            if (index >= timeline.lineColors.length) {
                i = index % timeline.lineColors.length
            }
            return timeline.lineColors[i]
        }

        function updateIndicatorColor(): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let lineIndicator = timeline.sectionsList[i].lineIndicator
                lineIndicator.color = getColorAtIndex(i)
            }
        }

        function updateLastHandle(): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                timeline.sectionsList[i].lineHandle.isLast = i === timeline.sectionsList.length - 1
                timeline.sectionsList[i].lineIndicator.isLast = i === timeline.sectionsList.length - 1
            }
        }

        function updateIndicatorPositions(): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                const lineIndicator = timeline.sectionsList[i].lineIndicator
                //console.debug(i, lineIndicator.x, lineIndicator.width, timeline.sectionsList[i].relativeLinePosition)
                const handle = timeline.sectionsList[i].lineHandle
                // Left edge = this section's left drag bound (previous handle
                // + minGap) converted to pixels; right edge = this handle's x.
                // Both derive from the seconds model — no pixel state to drift.
                lineIndicator.x = handle.minSeconds / 86400 * handle.lineWidth
                lineIndicator.width = handle.x - lineIndicator.x
            }
        }

        Rectangle {
            id: addHandleWrapper
            objectName: root.objectNamePrefix + "AddArea"
            color: Material.theme === Material.Dark ? Material.color(Material.Grey, Material.Shade900) : Material.color(Material.Grey, Material.Shade100)
            height: 30
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            HoverHandler {
                id: hoverHandler
                enabled: true
            }

            // Current time indicator
            Rectangle {
                id: currentTimeIndicator
                color: Material.theme === Material.Dark ? Qt.alpha(Material.color(Material.BlueGrey), 0.5) : Qt.alpha(Material.color(Material.BlueGrey, Material.Shade400), 0.6)
                width: 2
                height: 30
                y: (addHandleWrapper.height - height) / 2 // Vertically center within addHandleWrapper

                property int totalSeconds: 86400 // Total seconds in a day
                property int currentSeconds: 0

                x: addHandleWrapper.width * (currentSeconds / totalSeconds)

                Timer {
                    interval: 1000
                    repeat: true
                    running: true
                    onTriggered: {
                        currentTimeIndicator.currentSeconds = (new Date().getHours() * 3600) + (new Date().getMinutes() * 60) + new Date().getSeconds()
                        currentTimeIndicator.x = addHandleWrapper.width * (currentTimeIndicator.currentSeconds / currentTimeIndicator.totalSeconds)
                        currentTimeText.text = Qt.formatTime(new Date(), "hh:mm:ss")
                    }
                }
            }

            Text {
                id: currentTimeText
                color: Material.theme === Material.Dark ? Material.color(Material.Grey) : Material.color(Material.Grey, Material.Shade700)
                text: Qt.formatTime(new Date(), "hh:mm:ss")
                font.pointSize: 12
                anchors {
                    bottom: addHandleWrapper.top
                    horizontalCenter: currentTimeIndicator.horizontalCenter
                }
            }

            Item {
                id: timelineIndicatorWrapper
                height: 30
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }
                Text {
                    color: "gray"
                    text: "0"
                    anchors {
                        horizontalCenter: zeroIndicator.horizontalCenter
                    }
                }

                Rectangle {
                    id: zeroIndicator
                    color: "gray"
                    width: 1
                    height: 10
                    anchors {
                        right: timelineIndicatorWrapper.left
                        bottom: timelineIndicatorWrapper.bottom
                    }
                }
                Repeater {
                    id: timelineIndicator
                    model: 24
                    Item {
                        id: timelineIndicatorItem
                        width: timelineIndicatorWrapper.width / timelineIndicator.count
                        height: 30
                        required property int index
                        x: timelineIndicatorItem.index * width
                        Text {
                            id: txtHours
                            color: Material.theme === Material.Dark ? Material.color(Material.Grey) : Material.color(Material.Grey, Material.Shade800)
                            text: timelineIndicatorItem.index + 1
                            anchors {
                                horizontalCenter: indicator.horizontalCenter
                            }
                        }

                        Rectangle {
                            id: indicator
                            color: Material.theme === Material.Dark ? Material.color(Material.Grey) : Material.color(Material.Grey, Material.Shade700)
                            width: 1
                            height: 10
                            anchors {
                                right: parent.right
                                bottom: parent.bottom
                            }
                        }

                        // 15-minute sub-ticks between hour marks: 3 minor ticks
                        // per hour at 1/4, 2/4, 3/4 of the hour cell. Shorter
                        // and lighter than the hour indicator so the hour
                        // boundaries still read as primary.
                        Repeater {
                            model: 3
                            Rectangle {
                                required property int index
                                color: Material.theme === Material.Dark ? Material.color(Material.Grey, Material.Shade700) : Material.color(Material.Grey, Material.Shade500)
                                width: 1
                                height: 5
                                x: timelineIndicatorItem.width * (index + 1) / 4
                                anchors.bottom: parent.bottom
                            }
                        }
                    }
                }
            }

            ToolButton {
                id: btnAdd
                objectName: root.objectNamePrefix + "BtnAdd"
                text: "➕"
                enabled: !App.globalVariables.isBasicVersion()
                visible: enabled
                onClicked: {
                    const absTimelinePosX = btnAdd.x + width * .5
                    const raw = absTimelinePosX / timeline.width
                    // Match drag behaviour: default snaps to 15 min, Shift
                    // (freeDragModifier) loosens to 1 min. Keeps the inserted
                    // boundary aligned with the same grid the user just saw
                    // their cursor hovering on top of.
                    const divisor = root.freeDragModifier ? 1440 : 96
                    const snapped = Math.round(raw * divisor) / divisor
                    const position = Number(snapped).toFixed(6)
                    timeline.createTimelineAt(position)
                }

                ScreenPlayProPopup {
                    id: screenPlayProView
                    modalSource: root.modalSource
                }

                // Default to centre when the user has not hovered yet; once the
                // pointer enters addHandleWrapper, follow it. Without this fallback
                // the button starts off-screen left (hoverHandler reports x=0).
                x: hoverHandler.hovered
                    ? hoverHandler.point.position.x - width * .5
                    : addHandleWrapper.width * 0.5 - width * .5
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item {
            id: lineIndicatorWrapper
            height: 5
            anchors {
                left: parent.left
                right: parent.right
                top: addHandleWrapper.bottom
            }
        }
        Item {
            id: handleWrapper
            height: 5
            anchors {
                left: parent.left
                right: parent.right
                top: addHandleWrapper.bottom
            }
        }
        Rectangle {
            height: 20
            color: Material.theme === Material.Dark ? Material.color(Material.Grey) : Material.color(Material.Grey, Material.Shade700)
            width: 1
            anchors {
                right: parent.left
                verticalCenter: lineIndicatorWrapper.verticalCenter
            }
        }
        Rectangle {
            height: 20
            width: 1
            color: Material.theme === Material.Dark ? Material.color(Material.Grey) : Material.color(Material.Grey, Material.Shade700)
            anchors {
                right: parent.right
                verticalCenter: lineIndicatorWrapper.verticalCenter
            }
        }

        ToolButton {
            id: btnReset
            objectName: root.objectNamePrefix + "BtnReset"
            text: resetting ? qsTr("Reseting...") : qsTr("❌ Reset")
            property bool resetting: false
            enabled: !resetting
            visible: !App.globalVariables.isBasicVersion()
            anchors {
                right: parent.right
                top: parent.top
            }
            onClicked: {
                console.log(LoggingCategories.timeline, "resetting", btnReset.resetting)
                btnReset.resetting = true
                App.screenPlayManager.removeAllRunningWallpapers().then(result => {
                    if (!result.success) {
                        InstantPopup.openErrorPopup(timeline, result.message)
                        btnReset.resetting = false
                        return
                    }
                    App.screenPlayManager.removeAllTimlineSections().then(result => {
                        btnReset.resetting = false
                        if (!result.success) {
                            InstantPopup.openErrorPopup(timeline, result.message)
                            return
                        }
                        timeline.removeAll()
                        const position = 1.0
                        timeline.createTimelineAt(position)
                    })
                })
            }
            anchors {
                right: parent.right
                top: parent.top
                topMargin: -height
            }
        }
    }
}
