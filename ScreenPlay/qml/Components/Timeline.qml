import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Control {
    id: root
    height: 160
    implicitWidth: 800
    topPadding: 20
    leftPadding: 20
    rightPadding: 20

    function removeAll(){
        timeLine.removeAll()
    }
    contentItem: Item {
        id: timeLine

        property var sectionsList: []
        property var lineColors: ["#E53935", "#D81B60", "#8E24AA", "#5E35B1", "#3949AB", "#1E88E5", "#00897B", "#43A047", "#C0CA33", "#FFB300", "#FB8C00", "#F4511E"]
        onWidthChanged: timeLine.updatePositions()

        Component.onCompleted: {
            init()
        }

        function init(){
            const initialStopPositions = [1]
            createAllSections(initialStopPositions)
        }

        function removeAll(){
            print("removeAll",timeLine.sectionsList.length)


            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                // ORDER is important here! Destory the children first
                print("remove index ", i)
                let section = timeLine.sectionsList[i]
                section.lineHandle.destroy()
                section.lineIndicator.destroy()
                section.destroy()
            }
            timeLine.sectionsList = []

            init()
        }

        function createAllSections(initialStopPositions) {
            for (let index in initialStopPositions) {
                addSection(initialStopPositions[Number(index)])
            }
        }

        function addSection(stopPosition) {
            print("stopPosition", stopPosition)
            //if (!isFloat(stopPosition))
            //    console.error(typeof (stopPosition), stopPosition)
            // Make sure to limit float precision
            const fixedStopPosition = stopPosition
            print("addSection at: ", fixedStopPosition)
            if (stopPosition < 0 || fixedStopPosition > 1) {
                console.error("Invalid position:", fixedStopPosition)
                return
            }

            let sectionComponent = Qt.createComponent("Section.qml")
            if (sectionComponent.status === Component.Error) {
                console.error(sectionComponent.errorString())
                return
            }
            let sectionObject = sectionComponent.createObject(timeLine, {
                                                                  "relativeLinePosition": fixedStopPosition
                                                              })
            timeLine.sectionsList.push(sectionObject)

            timeLine.sectionsList.sort(function (a, b) {
                return a.relativeLinePosition - b.relativeLinePosition
            })
            const index = timeLine.sectionsList.indexOf(sectionObject)
            console.log("Addsection:", index)
            createSection(index, fixedStopPosition, sectionObject)

            updatePositions()
        }

        function createSection(index, stopPosition, section) {
            console.log("Adding at:", index, stopPosition)

            //console.assert(isFloat(stopPosition))
            let haComponent = Qt.createComponent("LineHandle.qml")
            if (haComponent.status === Component.Error) {
                console.assert(haComponent.errorString())
                return
            }

            section.lineHandle = haComponent.createObject(handleWrapper)
            section.lineHandle.lineWidth = timeLine.width
            section.lineHandle.x = Math.round(
                        handleWrapper.width * timeLine.sectionsList[index].relativeLinePosition)
            section.lineHandle.y = -section.lineHandle.height / 2
            // Will be set later
            section.lineHandle.lineMinimum = timeLine.x
            section.lineHandle.lineMaximum = timeLine.x
            section.lineHandle.handleMoved.connect(timeLine.updatePositions)

            let liComponent = Qt.createComponent("LineIndicator.qml")
            if (liComponent.status === Component.Error) {
                console.assert(liComponent.errorString())
                return
            }

            // Set color initially so we do not have a weird color animation at start
            const lineIndicatorProperties = {
                "color": getColorAtIndex(index)
            }
            section.lineIndicator = liComponent.createObject(
                        lineIndicatorWrapper, lineIndicatorProperties)
            section.lineIndicator.height = lineIndicatorWrapper.height
            section.lineIndicator.index = index
            section.lineIndicator.color = getColorAtIndex(index)
            section.lineIndicator.remove.connect(timeLine.removeSection)
            section.lineIndicator.lineSelected.connect(
                        timeLine.lineIndicatorSelected)
        }

        function lineIndicatorSelected(selectedIndicatorindex) {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                if (i === selectedIndicatorindex) {
                    timeLine.sectionsList[i].lineIndicator.selected = true
                    continue
                }
                timeLine.sectionsList[i].lineIndicator.selected = false
            }
        }

        // We must update all indexes when removing/adding an element
        function updateIndicatorIndexes() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                timeLine.sectionsList[i].index = i
                timeLine.sectionsList[i].lineIndicator.index = i
            }
        }

        function removeSection(index) {
            print(timeLine.stopPositionList)
            print(timeLine.sectionList)

            const isLast = index === timeLine.sectionsList.length - 1
            if (isLast)
                return
            // ORDER is important here! First destory the object
            // and then remove i f
            let section = timeLine.sectionsList[index]
            section.lineHandle.destroy()
            section.lineIndicator.destroy()
            section.destroy()
            timeLine.sectionsList.splice(index, 1)

            updatePositions()
        }

        function updatePositions() {
            // Iterate through each handle in the 'sectionList' array
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                let handle = timeLine.sectionsList[i].lineHandle

                // Determine the minimum position for the current handle
                let prevPos
                if (i === 0) {
                    // If it's the first handle, its minimum is 0
                    prevPos = 0
                } else {
                    // Otherwise, it's directly the position of the previous handle
                    prevPos = timeLine.sectionsList[i - 1].lineHandle.x
                }

                // Determine the maximum position for the current handle
                let nextPos
                if (i === timeLine.sectionsList.length - 1) {
                    // If it's the last handle, its maximum is the width of the line
                    nextPos = timeLine.width
                } else {
                    // Otherwise, it's directly the position of the next handle
                    nextPos = timeLine.sectionsList[i + 1].lineHandle.x
                }

                // Set the determined minimum and maximum positions for the current handle
                handle.lineMinimum = prevPos
                handle.lineMaximum = nextPos
            }
            updateIndicatorIndexes()
            updateIndicatorPositions()
            updateLastHandle()
            updateIndicatorColor()
        }

        function getColorAtIndex(index) {
            let i = index
            // Start from the beginnging again
            if (index >= timeLine.lineColors.length) {
                i = index % timeLine.lineColors.length
            }
            return timeLine.lineColors[i]
        }

        function updateIndicatorColor() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                let lineIndicator = timeLine.sectionsList[i].lineIndicator
                lineIndicator.color = getColorAtIndex(i)
            }
        }

        function updateLastHandle() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                timeLine.sectionsList[i].lineHandle.isLast = i === timeLine.sectionsList.length - 1
                timeLine.sectionsList[i].lineIndicator.isLast = i
                        === timeLine.sectionsList.length - 1
            }
        }

        function updateIndicatorPositions() {
            for (let i = 0; i < timeLine.sectionsList.length; i++) {
                const lineIndicator = timeLine.sectionsList[i].lineIndicator
                print(i, lineIndicator.x, lineIndicator.width)
                const handle = timeLine.sectionsList[i].lineHandle
                lineIndicator.x = handle.dragHandler.xAxis.minimum
                lineIndicator.width = (handle.linePosition * handle.lineWidth).toFixed(
                            2) - lineIndicator.x
            }
        }

        // https://stackoverflow.com/a/3885844
        function isFloat(n) {
            return n === +n && n !== (n | 0)
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

            RowLayout {
                anchors.fill: parent
                Repeater {
                    model: 24
                    Item {
                        width: 20
                        height: 40
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
                    const p = this.x / timeLine.width
                    const position = p.toFixed(2)
                    timeLine.addSection(position)
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
