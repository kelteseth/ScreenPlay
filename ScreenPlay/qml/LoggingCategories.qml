pragma Singleton

import QtQml

/*! 
    \qmltype LoggingCategories
    \brief Singleton containing all QML logging categories.
    
    This singleton provides centralized logging categories for the entire application.
    Using a singleton is more efficient than creating LoggingCategory objects in each component.
    
    \note Always use this singleton instead of creating local LoggingCategory instances.
    
    Example usage:
    \qml
    import ScreenPlay
    
    Item {
        Component.onCompleted: {
            console.log(LoggingCategories.monitorSelection, "Monitor selected")
            console.warn(LoggingCategories.timeline, "Timeline warning")
        }
    }
    \endqml
*/
QtObject {
    id: root
    readonly property LoggingCategory monitorSelection: LoggingCategory {
        name: "monitorSelection"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory timeline: LoggingCategory {
        name: "timeline"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory lineHandle: LoggingCategory {
        name: "lineHandle"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory lineIndicator: LoggingCategory {
        name: "lineIndicator"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory timelineSection: LoggingCategory {
        name: "timelineSection"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory contentSettings: LoggingCategory {
        name: "contentSettings"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory videoImport: LoggingCategory {
        name: "videoImport"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory monitorProjectSettings: LoggingCategory {
        name: "monitorProjectSettings"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory monitorSelectionItem: LoggingCategory {
        name: "monitorSelectionItem"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory navigation: LoggingCategory {
        name: "navigation"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory installed: LoggingCategory {
        name: "installed"
        defaultLogLevel: LoggingCategory.Debug
    }
    readonly property LoggingCategory videoControls: LoggingCategory {
        name: "videoControls"
        defaultLogLevel: LoggingCategory.Debug
    }
}
