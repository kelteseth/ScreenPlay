.pragma library
.import QtQuick as QtQ

var errorPopup


/*
  Creates error popups in js. Make sure you are parenting to an item.
  Alternativly use  InstantPopup.openErrorPopup(Window.window.contentItem, message);
  We cannot access window in here, so the developer must provide it...
*/
function openErrorPopup(parent, errorMsg) {
    // Check if parent is a valid QML Item
    if (!(parent instanceof QtQ.Item)) {
        console.error(
                    "Error: Invalid parent object provided. Parent must be a QML Item or inherit from it.",
                    typeof (parent))
        return
    }

    if (!errorPopup) {
        var errorComponent = Qt.createComponent(
            "qrc:/qt/qml/ScreenPlayCore/qml/ErrorPopup.qml")
        if (errorComponent.status === QtQ.Component.Ready) {
            errorPopup = errorComponent.createObject(parent)
        } else {
            console.error("Error creating ErrorPopup:",
                          errorComponent.errorString())
            return
        }
    }
    errorPopup.errorMessage = errorMsg
    errorPopup.open()
}
