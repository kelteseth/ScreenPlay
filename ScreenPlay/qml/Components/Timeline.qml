pragma ValueTypeBehavior

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlayUtil

Control {
    id: root
    height: 160
    implicitWidth: 600
    topPadding: 20
    leftPadding: 20
    rightPadding: 20

    // User selected
    property int selectedTimelineIndex: -1
    property var selectedTimeline: timeline.sectionsList[root.selectedTimelineIndex]
    property int length: timeline.sectionsList.length

    function removeAll() {
        timeline.removeAll();
    }

    function reset() {
        timeline.reset();
    }
    LoggingCategory {
        id: timelineLogging
        name: "timeline"
        defaultLogLevel: LoggingCategory.Debug
    }

    Connections {
        target: App.screenPlayManager
        function onPrintQmlTimeline() {
            console.debug(timelineLogging, "################# qml:");
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                console.debug(timelineLogging, timeline.sectionsList[i].index, timeline.sectionsList[i].identifier, timeline.sectionsList[i].relativeLinePosition);
            }
        }
    }

    Component {
        id: sectionComp
        QtObject {
            property string identifier
            property int index: 0
            property real relativeLinePosition: lineHandle.linePosition
            onRelativeLinePositionChanged: console.debug("relativelinepos: ", relativeLinePosition)
            property LineHandle lineHandle
            property LineIndicator lineIndicator
        }
    }

    contentItem: Item {
        id: timeline
        height: 160
        implicitWidth: 600

        property var sectionsList: []
        property var lineColors: ["#1E88E5", "#00897B", "#43A047", "#C0CA33", "#FFB300", "#FB8C00", "#F4511E", "#E53935", "#D81B60", "#8E24AA", "#5E35B1", "#3949AB"]

        onWidthChanged: timeline.updatePositions()
        Component.onCompleted: reset()

        Timer {
            running: true
            repeat: true
            interval: 500
            onTriggered: {
                const activeTimelineIndex = App.screenPlayManager.activeTimelineIndex();
                if (activeTimelineIndex === -1) {
                    return;
                }
                for (var i = 0; i < timeline.sectionsList.length; i++) {
                    let section = timeline.sectionsList[i];
                    section.lineIndicator.isActive = (activeTimelineIndex === i);
                }
            }
        }

        function reset() {
            removeAll();
            let initialSectionsList = App.screenPlayManager.initialSectionsList();
            if (App.globalVariables.isBasicVersion()) {
                if (initialSectionsList.length > 1) {
                    console.error(timelineLogging, "Invalid section list count for basic version");
                    // Create dummy
                    addSection("INVALID", 1);
                }
                return;
            }
            initialSectionsList.sort(function (a, b) {
                return b.index - a.index;
            });
            for (let index in initialSectionsList) {
                let section = initialSectionsList[index];
                addSection(section.identifier, section.relativePosition);
            }
        }

        function removeAll() {
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
            root.selectedTimelineIndex = -1;
        }

        // IMPORTANT: The new element is always on the left. The first
        // handle always persists  because the
        // user can never delete it. It only gets "pushed" further
        // to the right, by decreasing its size.
        function addSection(identifier, stopPosition) {
            console.debug(timelineLogging, "stopPosition", stopPosition);

            // Make sure to limit float precision
            const fixedStopPosition = stopPosition;
            console.debug(timelineLogging, "addSection at: ", fixedStopPosition);
            if (stopPosition < 0 || fixedStopPosition > 1) {
                console.error(timelineLogging, "Invalid position:", fixedStopPosition);
                return;
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
            return sectionObject;
        }

        function createSection(index, stopPosition, section, identifier) {
            console.debug(timelineLogging, "Adding at:", index, stopPosition, identifier);

            //console.assert(isFloat(stopPosition))
            let haComponent = Qt.createComponent("LineHandle.qml");
            if (haComponent.status === Component.Error) {
                console.assert(timelineLogging, haComponent.errorString());
                return;
            }
            section.lineHandle = haComponent.createObject(handleWrapper);
            section.lineHandle.lineWidth = timeline.width;
            section.lineHandle.x = Math.round(handleWrapper.width * timeline.sectionsList[index].relativeLinePosition);
            section.lineHandle.y = -section.lineHandle.height / 2;
            // Will be set later
            section.lineHandle.lineMinimum = timeline.x;
            section.lineHandle.lineMaximum = timeline.x;
            section.lineHandle.handleMoved.connect(timeline.onHandleMoved);
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

        function sectionFromHandle(lineHandle) {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                if (timeline.sectionsList[i].lineHandle === lineHandle)
                    return timeline.sectionsList[i];
            }
            return null;
        }

        function onHandleMoved(lineHandle) {
            updatePositions();
            const section = sectionFromHandle(lineHandle);
            if (section === null) {
                console.debug(timelineLogging, lineHandle.linePosition);
                console.error(timelineLogging, "Unable to match handle to section list");
                return;
            }
            App.screenPlayManager.moveTimelineAt(section.index, section.identifier, lineHandle.linePosition, lineHandle.timeString);
        }

        function lineIndicatorSelected(selectedTimelineIndex) {
            console.debug(timelineLogging, "selectedTimelineIndex:", selectedTimelineIndex, "section cout: ", timeline.sectionsList.length);
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                if (i === selectedTimelineIndex) {
                    timeline.sectionsList[i].lineIndicator.selected = true;
                    continue;
                }
                timeline.sectionsList[i].lineIndicator.selected = false;
            }
            root.selectedTimelineIndex = selectedTimelineIndex;
            App.screenPlayManager.setSelectedTimelineIndex(selectedTimelineIndex);
        }

        // We must update all indexes when removing/adding an element
        function updateIndicatorIndexes() {
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

        function removeSection(index) {
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
            App.screenPlayManager.removeTimelineAt(index);
        }

        function updatePositions() {
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

                //timeline.sectionsList[i].relativeLinePosition =prevPos / timeline.width
                // console.debug("sections: ", i, "prev minimum ",prevPos,"next maximum", nextPos,  timeline.sectionsList[i].relativeLinePosition)
            }
            for (var j = 0; j < timeline.sectionsList.length; j++) {
                let section = timeline.sectionsList[j];
                section.relativeLinePosition = section.lineHandle.linePosition;
                // console.debug(section.relativeLinePosition, section.lineHandle.lineMinimum, section.lineHandle.lineMaximum)
            }
            updateIndicatorPositions();
            updateLastHandle();
            updateIndicatorColor();
            updateIndicatorIndexes();
        }

        function getColorAtIndex(index) {
            let i = index;
            // Start from the beginnging again
            if (index >= timeline.lineColors.length) {
                i = index % timeline.lineColors.length;
            }
            return timeline.lineColors[i];
        }

        function updateIndicatorColor() {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                let lineIndicator = timeline.sectionsList[i].lineIndicator;
                lineIndicator.color = getColorAtIndex(i);
            }
        }

        function updateLastHandle() {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                timeline.sectionsList[i].lineHandle.isLast = i === timeline.sectionsList.length - 1;
                timeline.sectionsList[i].lineIndicator.isLast = i === timeline.sectionsList.length - 1;
            }
        }

        function updateIndicatorPositions() {
            for (var i = 0; i < timeline.sectionsList.length; i++) {
                const lineIndicator = timeline.sectionsList[i].lineIndicator;
                //console.debug(i, lineIndicator.x, lineIndicator.width, timeline.sectionsList[i].relativeLinePosition)
                const handle = timeline.sectionsList[i].lineHandle;
                lineIndicator.x = handle.dragHandler.xAxis.minimum;
                lineIndicator.width = (handle.linePosition * handle.lineWidth).toFixed(2) - lineIndicator.x;
            }
        }

        // https://stackoverflow.com/a/3885844
        function isFloat(n) {
            return n === +n && n !== (n | 0);
        }

        Rectangle {
            id: addHandleWrapper
            color: Material.color(Material.Grey, Material.Shade900)
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
                color: Material.color(Material.BlueGrey)
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
                color: Material.color(Material.Grey)
                text: Qt.formatTime(new Date(), "hh:mm:ss")
                font.pointSize: 12
                anchors {
                    bottom: addHandleWrapper.top
                    horizontalCenter: currentTimeIndicator.horizontalCenter
                }
            }

            RowLayout {
                height: 30
                uniformCellSizes: true
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    leftMargin: -5
                }
                Repeater {
                    model: 25
                    Item {
                        width: 20
                        height: 30
                        required property int index
                        Text {
                            id: txtHours
                            color: "gray"
                            text: index
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Rectangle {
                            color: "gray"
                            width: 1
                            height: 10
                            anchors {
                                horizontalCenter: txtHours.horizontalCenter
                                bottom: parent.bottom
                            }
                        }
                    }
                }
            }

            ToolButton {
                text: "➕"
                onClicked: {
                    if (App.globalVariables.isBasicVersion()) {
                        screenPlayProView.open();
                        return;
                    }
                    const p = this.x / timeline.width;
                    const position = p.toFixed(4);
                    const identifier = App.util.generateRandomString(4);
                    const sectionObject = timeline.addSection(identifier, position);
                    App.screenPlayManager.addTimelineAt(sectionObject.index, sectionObject.relativeLinePosition, sectionObject.identifier);
                }
                ScreenPlayProPopup {
                    id: screenPlayProView
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
            height: 18
            color: "#757575"
            width: 2
            anchors {
                right: parent.left
                verticalCenter: lineIndicatorWrapper.verticalCenter
            }
        }
        Rectangle {
            height: 18
            width: 2
            color: "#757575"
            anchors {
                right: parent.right
                verticalCenter: lineIndicatorWrapper.verticalCenter
            }
        }

        ToolButton {
            text: "❌ Reset" //qsTr("Remove all timeline ranges")
            z: 99
            anchors {
                right: parent.right
                top: parent.top
            }
            onClicked: {
                timeline.removeAll();
                App.screenPlayManager.removeAllTimlineSections().then(result => {
                    if (!result.success) {
                        console.error("removeAllTimlineSections failed");
                        return;
                    }
                    const position = 1.0;
                    const identifier = App.util.generateRandomString(4);
                    const sectionObject = timeline.addSection(identifier, position);
                    App.screenPlayManager.addTimelineAt(sectionObject.index, sectionObject.relativeLinePosition, sectionObject.identifier);
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
