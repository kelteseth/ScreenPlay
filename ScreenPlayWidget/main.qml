import QtQuick 2.9
import QtQuick.Window 2.2
import net.aimber.screenplaysdk 1.0

Window {
    id: mainWindow
    visible: true
    width: 250
    height: 100
    color: "transparent"
    flags: Qt.SplashScreen | Qt.ToolTip | Qt.WindowStaysOnBottomHint

    Loader {
        id:loader
        anchors.fill: parent
        asynchronous: true

    }

}
