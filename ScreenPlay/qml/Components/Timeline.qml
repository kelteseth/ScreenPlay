pragma ValueTypeBehavior

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

Control {
    id: root
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
    property int length: timeline.sectionsList.length
    property Item modalSource

    function getSelectedTimeline(): TimelineSection {
        return timeline.sectionsList[root.selectedTimelineIndex];
    }

    function setActiveWallpaperPreviewImage(): void {
        timeline.setActiveWallpaperPreviewImage();
    }

    function removeAll(): void {
        timeline.removeAll();
    }

    function reset(): void {
        timeline.reset();
        timeline.load();
        root.ready();
    }

    LoggingCategory {
        id: timelineLogging
        name: "timeline"
        defaultLogLevel: LoggingCategory.Debug
    }

    Connections {
        target: App.screenPlayManager
        function onPrintQmlTimeline(): void {
            console.debug(timelineLogging, "################# qml:");
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                console.debug(timelineLogging, timeline.sectionsList[i].index, timeline.sectionsList[i].identifier, timeline.sectionsList[i].relativeLinePosition);
            }
        }
    }

    contentItem: Item {
        id: timeline
        height: 160
        implicitWidth: 600

        property var sectionsList: []
        property var lineColors: ["#1E88E5", "#00897B", "#43A047", "#C0CA33", "#FFB300", "#FB8C00", "#F4511E", "#E53935", "#D81B60", "#8E24AA", "#5E35B1", "#3949AB"]

        // ⚠️ Note: We need to update the lineWidth of every handle, because
        // at startup the timeline.width gets updated multiple time. This caused
        // a nasty bug where the initial lineHandle.lineWidth was set to
        // the implicitWidth of 600, but the actual timeine width was 608 causing
        // issues with handle movement dragging across boundries.
        function load(): void {
            timeline.reset();
            for (var i = 0; i < sectionsList.length; i++) {
                sectionsList[i].lineHandle.lineWidth = width;
            }
            timeline.updatePositions();
            timeline.updateActiveTimelineIndex(App.screenPlayManager.activeTimelineIndex);
        }

        // Component.onCompleted: reset()

        Connections {
            target: App.screenPlayManager
            function onNotifyUiReloadTimelinePreviewImage(): void {
                timeline.setActiveWallpaperPreviewImage();
            }
            function onActiveTimelineIndexChanged(activeTimelineIndex: int): void {
                timeline.updateActiveTimelineIndex(activeTimelineIndex);
            }
        }

        function updateActiveTimelineIndex(activeTimelineIndex: int): bool {
            if (activeTimelineIndex === -1) {
                return false;
            }
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let section = timeline.sectionsList[i];
                section.lineIndicator.isActive = (activeTimelineIndex === i);
            }
            return true;
        }

        function createTimelineAt(relativePosition: real): bool {
            const identifier = App.util.generateRandomString(4);
            const sectionIndex = timeline.addSection(identifier, relativePosition);
            const sectionObject = timeline.sectionsList[sectionIndex];
            const addTimelineAtSuccess = App.screenPlayManager.addTimelineAt(sectionObject.index, sectionObject.relativeLinePosition, sectionObject.identifier);
            if (!addTimelineAtSuccess) {
                InstantPopup.openErrorPopup(timeline, qsTr("Unable to add Timeline"));
                return false;
            }
            return true;
        }

        function reset(): void {
            console.warn("⚠️ RESET");
            removeAll();
            let initialSectionsList = App.screenPlayManager.timelineSections();
            initialSectionsList.sort(function (a, b) {
                return b.index - a.index;
            });
            for (let index in initialSectionsList) {
                let section = initialSectionsList[index];
                addSection(section.identifier, section.relativePosition);
            }
            const activeTimelineIndex = App.screenPlayManager.activeTimelineIndex;
            if (activeTimelineIndex === -1) {
                return;
            }
            lineIndicatorSelected(activeTimelineIndex);
            setActiveWallpaperPreviewImage();
        }

        function setActiveWallpaperPreviewImage(): void {
            if (timeline.sectionsList.length == 0) {
                console.error("Cannot set preview image with an empty section list");
                return;
            }

            let timelineSectionList = App.screenPlayManager.timelineSections();
            for (var i = 0; i < timelineSectionList.length; i++) {
                let timelineSection = timelineSectionList[i];
                if (!timeline.sectionsList[i]) {
                    console.error("No sectionList item at index:", i, " of ", timeline.sectionsList.length);
                    return;
                }

                let lineIndicator = timeline.sectionsList[i].lineIndicator;

                // Reset the preview image
                if (timelineSection.wallpaperData.length === 0) {
                    lineIndicator.wallpaperPreviewImage = "";
                    continue;
                }

                let firstWallpaper = timelineSection.wallpaperData[0];
                lineIndicator.wallpaperPreviewImage = Qt.resolvedUrl("file:///" + firstWallpaper.absolutePath + "/" + firstWallpaper.previewImage);
            }
        }

        function removeAll(): void {
            console.debug(timelineLogging, "removeAll", timeline.sectionsList.length);
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                // ORDER is important here! Destory the children first
                console.debug(timelineLogging, "remove index ", i);
                let section = timeline.sectionsList[i];
                section.lineHandle.destroy();
                section.lineIndicator.destroy();
                section.destroy();
            }
            timeline.sectionsList = [];
            // Default to the first timeline
            root.selectedTimelineIndex = 0;
        }

        // IMPORTANT: The new element is always on the left. The first
        // handle always persists  because the
        // user can never delete it. It only gets "pushed" further
        // to the right, by decreasing its size.
        function addSection(identifier: string, stopPosition: real): int {
            console.debug(timelineLogging, "stopPosition", stopPosition);

            // Make sure to limit float precision
            const fixedStopPosition = stopPosition;
            console.debug(timelineLogging, "addSection at: ", fixedStopPosition);
            if (stopPosition < 0 || fixedStopPosition > 1) {
                console.error(timelineLogging, "Invalid position:", fixedStopPosition);
                return -1;
            }
            const sectionComp = Qt.createComponent("TimelineSection.qml");
            if (sectionComp.status === Component.Error) {
                console.assert(timelineLogging, sectionComp.errorString());
                return -1;
            }
            let sectionObject = sectionComp.createObject(timeline, {
                "identifier": identifier,
                "relativeLinePosition": fixedStopPosition
            });
            timeline.sectionsList.push(sectionObject);
            timeline.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition;
            });
            const index = timeline.sectionsList.indexOf(sectionObject);
            console.debug(timelineLogging, "Addsection:", index);
            createSection(index, fixedStopPosition, sectionObject, identifier);
            updatePositions();
            return index;
        }

        // Disables all other LineHandles if one is in use. Enables
        // all after the user released the LineHandle
        function setActiveHandle(identifier: string, active: bool): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let lineHandle = timeline.sectionsList[i].lineHandle;
                if (active) {
                    if (lineHandle.identifier !== identifier) {
                        lineHandle.otherLineHandleActive = true;
                    }
                } else {
                    lineHandle.otherLineHandleActive = false;
                }
            }
        }

        function createSection(index: int, stopPosition: real, section: TimelineSection, identifier: string): void {
            console.debug(timelineLogging, "Adding at:", index, stopPosition, identifier);

            let haComponent = Qt.createComponent("LineHandle.qml");
            if (haComponent.status === Component.Error) {
                console.assert(timelineLogging, haComponent.errorString());
                return;
            }
            section.lineHandle = haComponent.createObject(handleWrapper);
            section.lineHandle.lineWidth = timeline.width;
            section.lineHandle.x = Math.round(handleWrapper.width * timeline.sectionsList[index].relativeLinePosition);
            // Add vertical offset to make moving easier in the LineHandle.qml
            section.lineHandle.y = 10;
            // Will be set later
            section.lineHandle.lineMinimum = timeline.x;
            section.lineHandle.lineMaximum = timeline.x;
            section.lineHandle.identifier = identifier;
            section.lineHandle.handleMoved.connect(timeline.onHandleMoved);
            section.lineHandle.activated.connect(timeline.setActiveHandle); // Connect the new signal
            let liComponent = Qt.createComponent("LineIndicator.qml");
            if (liComponent.status === Component.Error) {
                console.assert(timelineLogging, liComponent.errorString());
                return;
            }

            // Set color initially so we do not have a weird color animation at start
            const lineIndicatorProperties = {
                "color": getColorAtIndex(index)
            };
            section.lineIndicator = liComponent.createObject(lineIndicatorWrapper, lineIndicatorProperties);
            section.lineIndicator.height = lineIndicatorWrapper.height;
            section.lineIndicator.index = index;
            section.lineIndicator.identifier = identifier;
            section.lineIndicator.color = getColorAtIndex(index);
            section.lineIndicator.remove.connect(timeline.removeSection);
            section.lineIndicator.lineSelected.connect(timeline.lineIndicatorSelected);
        }

        function sectionFromHandle(lineHandle: LineHandle): TimelineSection {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                if (timeline.sectionsList[i].lineHandle === lineHandle)
                    return timeline.sectionsList[i];
            }
            return null;
        }

        function onHandleMoved(lineHandle: LineHandle): void {
            updatePositions();
            const section = sectionFromHandle(lineHandle);
            if (section === null) {
                console.debug(timelineLogging, lineHandle.linePosition);
                console.error(timelineLogging, "Unable to match handle to section list");
                return;
            }
            App.screenPlayManager.moveTimelineAt(section.index, section.identifier, lineHandle.linePosition, lineHandle.timeString);
        }

        function lineIndicatorSelected(selectedTimelineIndex: int): void {
            console.debug(timelineLogging, "selectedTimelineIndex:", selectedTimelineIndex, "section cout: ", timeline.sectionsList.length);
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                const enableTimeline = (i === selectedTimelineIndex);
                timeline.sectionsList[i].lineIndicator.selected = enableTimeline;
            }
            root.selectedTimelineIndex = selectedTimelineIndex;
            App.screenPlayManager.setSelectedTimelineIndex(selectedTimelineIndex);
        }

        // We must update all indexes when removing/adding an element
        function updateIndicatorIndexes(): void {
            if (timeline.sectionsList === null || timeline.sectionsList === undefined)
                return;
            timeline.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition;
            });
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                timeline.sectionsList[i].index = i;
                timeline.sectionsList[i].lineIndicator.index = i;
                //console.debug("updateIndicatorIndexes:", timeline.sectionsList[i].index, timeline.sectionsList[i].relativeLinePosition)
            }
        }

        function removeSection(index: int): void {
            console.debug(timelineLogging, timeline.stopPositionList);
            console.debug(timelineLogging, timeline.sectionList);
            const isLast = index === timeline.sectionsList.length - 1;
            if (isLast)
                return;
            // ORDER is important here! First destory the object
            // and then remove i f
            let section = timeline.sectionsList[index];
            section.lineHandle.destroy();
            section.lineIndicator.destroy();
            section.destroy();
            timeline.sectionsList.splice(index, 1);
            updatePositions();
            App.screenPlayManager.removeTimelineAt(index).then(result => {
                if (!result.success) {
                    InstantPopup.openErrorPopup(timeline, result.message);
                    btnReset.resetting = false;
                    return;
                }
            });
        }

        function updatePositions(): void {
            // Iterate through each handle in the 'sectionList' array
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let handle = timeline.sectionsList[i].lineHandle;

                // Determine the minimum position for the current handle
                let prevPos;
                if (i === 0) {
                    // If it's the first handle, its minimum is 0
                    prevPos = 0;
                } else {
                    // Otherwise, it's directly the position of the previous handle
                    prevPos = timeline.sectionsList[i - 1].lineHandle.x;
                }

                // Determine the maximum position for the current handle
                let nextPos;
                if (i === timeline.sectionsList.length - 1) {
                    // If it's the last handle, its maximum is the width of the line
                    nextPos = timeline.width;
                } else {
                    // Otherwise, it's directly the position of the next handle
                    nextPos = timeline.sectionsList[i + 1].lineHandle.x;
                }

                // Set the determined minimum and maximum positions for the current handle
                handle.lineMinimum = prevPos;
                handle.lineMaximum = nextPos;
            }
            updateIndicatorPositions();
            updateLastHandle();
            updateIndicatorColor();
            updateIndicatorIndexes();
        }

        function getColorAtIndex(index: int): string { // Return hex not color
            let i = index;
            // Start from the beginnging again
            if (index >= timeline.lineColors.length) {
                i = index % timeline.lineColors.length;
            }
            return timeline.lineColors[i];
        }

        function updateIndicatorColor(): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let lineIndicator = timeline.sectionsList[i].lineIndicator;
                lineIndicator.color = getColorAtIndex(i);
            }
        }

        function updateLastHandle(): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                timeline.sectionsList[i].lineHandle.isLast = i === timeline.sectionsList.length - 1;
                timeline.sectionsList[i].lineIndicator.isLast = i === timeline.sectionsList.length - 1;
            }
        }

        function updateIndicatorPositions(): void {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                const lineIndicator = timeline.sectionsList[i].lineIndicator;
                //console.debug(i, lineIndicator.x, lineIndicator.width, timeline.sectionsList[i].relativeLinePosition)
                const handle = timeline.sectionsList[i].lineHandle;
                lineIndicator.x = handle.dragHandler.xAxis.minimum;
                lineIndicator.width = (handle.linePosition * handle.lineWidth).toFixed(2) - lineIndicator.x;
            }
        }

        Rectangle {
            id: addHandleWrapper
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
                        currentTimeIndicator.currentSeconds = (new Date().getHours() * 3600) + (new Date().getMinutes() * 60) + new Date().getSeconds();
                        currentTimeIndicator.x = addHandleWrapper.width * (currentTimeIndicator.currentSeconds / currentTimeIndicator.totalSeconds);
                        currentTimeText.text = Qt.formatTime(new Date(), "hh:mm:ss");
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
                    }
                }
            }

            ToolButton {
                id: btnAdd
                text: "➕"
                enabled: !App.globalVariables.isBasicVersion()
                visible: enabled
                onClicked: {
                    const absTimelinePosX = btnAdd.x + width * .5;
                    const position = Number(absTimelinePosX / timeline.width).toFixed(6);
                    timeline.createTimelineAt(position);
                }

                ScreenPlayProPopup {
                    id: screenPlayProView
                    modalSource: root.modalSource
                }

                x: hoverHandler.point.position.x - width * .5
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
            text: resetting ? qsTr("Reseting...") : qsTr("❌ Reset")
            property bool resetting: false
            enabled: !resetting
            visible: !App.globalVariables.isBasicVersion()
            anchors {
                right: parent.right
                top: parent.top
            }
            onClicked: {
                console.log("resetting", btnReset.resetting);
                btnReset.resetting = true;
                App.screenPlayManager.removeAllRunningWallpapers().then(result => {
                    if (!result.success) {
                        InstantPopup.openErrorPopup(timeline, result.message);
                        btnReset.resetting = false;
                        return;
                    }
                    App.screenPlayManager.removeAllTimlineSections().then(result => {
                        btnReset.resetting = false;
                        if (!result.success) {
                            InstantPopup.openErrorPopup(timeline, result.message);
                            return;
                        }
                        timeline.removeAll();
                        const position = 1.0;
                        timeline.createTimelineAt(position);
                    });
                });
            }
            anchors {
                right: parent.right
                top: parent.top
                topMargin: -height
            }
        }
    }
}
