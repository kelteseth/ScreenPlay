import QtQml

import ScreenPlay

QtObject {
    property string identifier
    property int index: 0
    // End position of this section as a relative 0..1 value. Set at creation
    // to the insert position — Timeline.addSection sorts the list *before* the
    // lineHandle exists, so this must be a plain stored value, not a binding
    // derived from the handle. Timeline.onHandleMoved refreshes it from
    // lineHandle.relativePosition (= endSeconds / 86400) after every drag.
    property real relativeLinePosition: 0
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
