import QtQml

import ScreenPlay

QtObject {
    property string identifier
    property int index: 0
    property real relativeLinePosition: lineHandle.linePosition
    onRelativeLinePositionChanged: console.debug(LoggingCategories.timelineSection, "relativelinepos: ", relativeLinePosition)
    property LineHandle lineHandle
    property LineIndicator lineIndicator

    function toString(): void {
        console.log(LoggingCategories.timelineSection, `TimelineEntry {
        index: ${index}
        identifier: ${identifier}
        relativeLinePosition: ${relativeLinePosition.toFixed(6)}
    }`)
    }
}
