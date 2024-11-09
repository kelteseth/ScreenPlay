.pragma library
.import QtQuick as QtQ

var errorPopup

/*
  Creates error popups in js
*/
function openErrorPopup(parent, errorMsg) {
    console.error(errorMsg)
    // Check if parent is a valid QML Item
    if (!(parent instanceof QtQ.Item)) {
        console.error(
                    "Error: Invalid parent object provided. Parent must be a QML Item or inherit from it.",
                    typeof (parent))
        return
    }

    if (!errorPopup) {
        var errorComponent = Qt.createComponent(
                    "qrc:/qml/ScreenPlayUtil/qml/ErrorPopup.qml")
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
