pragma ValueTypeBehavior: Addressable
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlayUtil

Control {
    id: root
    height: 160
    implicitWidth: 800
    topPadding: 20
    leftPadding: 20
    rightPadding: 20

    property int activeTimelineIndex: -1
    property int length: timeLine.sectionsList.length

    function getActiveTimeline() {
        return timeLine.sectionsList[root.activeTimelineIndex];
    }

    function removeAll() {
        timeLine.removeAll();
    }

    function printTimelines() {
        print("################# qml:");
        for (let i = 0; i < timeLine.sectionsList.length; i++) {
            print(timeLine.sectionsList[i].index, timeLine.sectionsList[i].identifier, timeLine.sectionsList[i].relativeLinePosition);
        }
    }

    Component {
        id: sectionComp
        QtObject {
            property string identifier
            property int index: 0
            property real relativeLinePosition: lineHandle.linePosition
            //onRelativeLinePositionChanged: print("relativelinepos: ", relativeLinePosition)
            property LineHandle lineHandle
            property LineIndicator lineIndicator
        }
    }

    contentItem: Item {
        id: timeLine

        property var sectionsList: []
        property var lineColors: ["#FFB300", "#FB8C00", "#F4511E", "#E53935", "#D81B60", "#8E24AA", "#5E35B1", "#3949AB", "#1E88E5", "#00897B", "#43A047", "#C0CA33"]
        onWidthChanged: timeLine.updatePositions()

        Component.onCompleted: {
            const initialStopPositions = App.screenPlayManager.initialStopPositions();
            createAllSections(initialStopPositions);
        }

        function createAllSections(initialStopPositions) {
            for (let identifier in initialStopPositions) {
                let stopPosition = initialStopPositions[identifier];
                addSection(identifier, stopPosition);
            }
        }

        function removeAll() {
            print("removeAll", timeLine.sectionsList.length);
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                // ORDER is important here! Destory the children first
                print("remove index ", i);
                let section = timeLine.sectionsList[i];
                section.lineHandle.destroy();
                section.lineIndicator.destroy();
                section.destroy();
            }
            timeLine.sectionsList = [];
            App.screenPlayManager.removeAllTimlineSections();
            const position = 1.0;
            const identifier = App.util.generateRandomString(4);
            const sectionObject = timeLine.addSection(identifier, position);
            App.screenPlayManager.addTimelineAt(sectionObject.index, sectionObject.relativeLinePosition, sectionObject.identifier);
        }

        // IMPORTANT: The new element is always on the left. The first
        // handle always persists  because the
        // user can never delete it. It only gets "pushed" further
        // to the right, by increasing the size.
        function addSection(identifier, stopPosition) {
            print("stopPosition", stopPosition);

            // Make sure to limit float precision
            const fixedStopPosition = stopPosition;
            print("addSection at: ", fixedStopPosition);
            if (stopPosition < 0 || fixedStopPosition > 1) {
                console.error("Invalid position:", fixedStopPosition);
                return;
            }
            let sectionObject = sectionComp.createObject(timeLine, {
                "identifier": identifier,
                "relativeLinePosition": fixedStopPosition
            });
            timeLine.sectionsList.push(sectionObject);
            timeLine.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition;
            });
            const index = timeLine.sectionsList.indexOf(sectionObject);
            console.log("Addsection:", index);
            createSection(index, fixedStopPosition, sectionObject, identifier);
            updatePositions();
            return sectionObject;
        }

        function createSection(index, stopPosition, section, identifier) {
            console.log("Adding at:", index, stopPosition, identifier);

            //console.assert(isFloat(stopPosition))
            let haComponent = Qt.createComponent("LineHandle.qml");
            if (haComponent.status === Component.Error) {
                console.assert(haComponent.errorString());
                return;
            }
            section.lineHandle = haComponent.createObject(handleWrapper);
            section.lineHandle.lineWidth = timeLine.width;
            section.lineHandle.x = Math.round(handleWrapper.width * timeLine.sectionsList[index].relativeLinePosition);
            section.lineHandle.y = -section.lineHandle.height / 2;
            // Will be set later
            section.lineHandle.lineMinimum = timeLine.x;
            section.lineHandle.lineMaximum = timeLine.x;
            section.lineHandle.handleMoved.connect(timeLine.onHandleMoved);
            let liComponent = Qt.createComponent("LineIndicator.qml");
            if (liComponent.status === Component.Error) {
                console.assert(liComponent.errorString());
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
            section.lineIndicator.remove.connect(timeLine.removeSection);
            section.lineIndicator.lineSelected.connect(timeLine.lineIndicatorSelected);
        }

        function sectionFromHandle(lineHandle) {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                if (timeLine.sectionsList[i].lineHandle === lineHandle)
                    return timeLine.sectionsList[i];
            }
            return null;
        }

        function onHandleMoved(lineHandle) {
            updatePositions();
            const section = sectionFromHandle(lineHandle);
            if (section === null) {
                print(lineHandle.linePosition);
                console.error("Unable to match handle to section list");
                return;
            }
            App.screenPlayManager.moveTimelineAt(section.index, section.identifier, lineHandle.linePosition, lineHandle.timeString);
        }

        function lineIndicatorSelected(activeTimelineIndex) {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                if (i === activeTimelineIndex) {
                    timeLine.sectionsList[i].lineIndicator.selected = true;
                    continue;
                }
                timeLine.sectionsList[i].lineIndicator.selected = false;
            }
            root.activeTimelineIndex = activeTimelineIndex;
        }

        // We must update all indexes when removing/adding an element
        function updateIndicatorIndexes() {
            if (timeLine.sectionsList === null || timeLine.sectionsList === undefined)
                return;
            timeLine.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition;
            });
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                timeLine.sectionsList[i].index = i;
                timeLine.sectionsList[i].lineIndicator.index = i;
                //print("updateIndicatorIndexes:", timeLine.sectionsList[i].index, timeLine.sectionsList[i].relativeLinePosition)
            }
        }

        function removeSection(index) {
            print(timeLine.stopPositionList);
            print(timeLine.sectionList);
            const isLast = index === timeLine.sectionsList.length - 1;
            if (isLast)
                return;
            // ORDER is important here! First destory the object
            // and then remove i f
            let section = timeLine.sectionsList[index];
            section.lineHandle.destroy();
            section.lineIndicator.destroy();
            section.destroy();
            timeLine.sectionsList.splice(index, 1);
            updatePositions();
            App.screenPlayManager.removeTimelineAt(index);
        }

        function updatePositions() {
            // Iterate through each handle in the 'sectionList' array
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                let handle = timeLine.sectionsList[i].lineHandle;

                // Determine the minimum position for the current handle
                let prevPos;
                if (i === 0) {
                    // If it's the first handle, its minimum is 0
                    prevPos = 0;
                } else {
                    // Otherwise, it's directly the position of the previous handle
                    prevPos = timeLine.sectionsList[i - 1].lineHandle.x;
                }

                // Determine the maximum position for the current handle
                let nextPos;
                if (i === timeLine.sectionsList.length - 1) {
                    // If it's the last handle, its maximum is the width of the line
                    nextPos = timeLine.width;
                } else {
                    // Otherwise, it's directly the position of the next handle
                    nextPos = timeLine.sectionsList[i + 1].lineHandle.x;
                }

                // Set the determined minimum and maximum positions for the current handle
                handle.lineMinimum = prevPos;
                handle.lineMaximum = nextPos;

                //timeLine.sectionsList[i].relativeLinePosition =prevPos / timeLine.width
                // print("sections: ", i, "prev minimum ",prevPos,"next maximum", nextPos,  timeLine.sectionsList[i].relativeLinePosition)
            }
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                let section = timeLine.sectionsList[i];
                section.relativeLinePosition = section.lineHandle.linePosition;
                // print(section.relativeLinePosition, section.lineHandle.lineMinimum, section.lineHandle.lineMaximum)
            }
            updateIndicatorPositions();
            updateLastHandle();
            updateIndicatorColor();
            updateIndicatorIndexes();
        }

        function getColorAtIndex(index) {
            let i = index;
            // Start from the beginnging again
            if (index >= timeLine.lineColors.length) {
                i = index % timeLine.lineColors.length;
            }
            return timeLine.lineColors[i];
        }

        function updateIndicatorColor() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                let lineIndicator = timeLine.sectionsList[i].lineIndicator;
                lineIndicator.color = getColorAtIndex(i);
            }
        }

        function updateLastHandle() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                timeLine.sectionsList[i].lineHandle.isLast = i === timeLine.sectionsList.length - 1;
                timeLine.sectionsList[i].lineIndicator.isLast = i === timeLine.sectionsList.length - 1;
            }
        }

        function updateIndicatorPositions() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                const lineIndicator = timeLine.sectionsList[i].lineIndicator;
                //print(i, lineIndicator.x, lineIndicator.width, timeLine.sectionsList[i].relativeLinePosition)
                const handle = timeLine.sectionsList[i].lineHandle;
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
                property int currentSeconds: (new Date().getHours() * 3600) + (new Date().getMinutes() * 60) + new Date().getSeconds()

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
                anchors.fill: parent
                Repeater {
                    model: 24
                    Item {
                        width: 20
                        height: 60
                        required property int index
                        Text {
                            color: "gray"
                            text: index
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Rectangle {
                            color: "gray"
                            width: 1
                            height: 5
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                bottom: parent.bottom
                            }
                        }
                    }
                }
            }

            ToolButton {
                text: "➕"
                onClicked: {
                    const p = this.x / timeLine.width;
                    const position = p.toFixed(4);
                    const identifier = App.util.generateRandomString(4);
                    const sectionObject = timeLine.addSection(identifier, position);
                    App.screenPlayManager.addTimelineAt(sectionObject.index, sectionObject.relativeLinePosition, sectionObject.identifier);
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
        Item {
            height: 18
            width: 5
            anchors {
                right: parent.left
                top: addHandleWrapper.bottom
                topMargin: -9
            }
        }
        Rectangle {
            height: 18
            width: 5
            color: "#757575"
            anchors {
                right: parent.right
                top: addHandleWrapper.bottom
                topMargin: -9
            }
        }
    }
}
