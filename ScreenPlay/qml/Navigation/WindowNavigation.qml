import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlay 1.0
import "../Workshop"
import "../Common" as Common

Rectangle {
    id: root
    height: 29
    implicitWidth: 1366
    color: Material.theme === Material.Light ? Material.background : "#242424"
    property Item modalSource
    property ApplicationWindow window

    Text {
        id: title
        text: qsTr("ScreenPlay Alpha %1 - Open Source Wallpaper And Widgets").arg(
                  ScreenPlay.version())
        color: Material.primaryTextColor
        verticalAlignment: Text.AlignVCenter

        anchors {
            top: parent.top
            left: parent.left
            leftMargin: 10
            bottom: parent.bottom
        }
    }

    MouseArea {
        id: mouseArea

        property var clickPos

        anchors.fill: parent
        hoverEnabled: true
        onPressed: function (mouse) {
            clickPos = {
                "x": mouse.x,
                "y": mouse.y
            }
        }
        onPositionChanged: {
            if (mouseArea.pressed) {
                let pos = ScreenPlay.cursorPos()
                window.setX(pos.x - clickPos.x)
                window.setY(pos.y - clickPos.y)
            }
        }
    }

    RowLayout {
        anchors {
            top: root.top
            right: root.right
            bottom: root.bottom
        }

        WindowNavButton {
            id: miMinimize
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/assets/icons/icon_minimize.svg"
            onClicked: root.window.hide()
        }
        WindowNavButton {
            id: miquit
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/assets/icons/icon_close.svg"
            onClicked: {
                if (ScreenPlay.screenPlayManager.activeWallpaperCounter === 0
                        && ScreenPlay.screenPlayManager.activeWidgetsCounter === 0) {
                    Qt.quit()
                    return
                }
                dialog.open()
            }
        }
    }

    Common.Dialog {
        id: dialog
        modalSource: root.modalSource
        title: qsTr("Are you sure you want to exit ScreenPlay? \nThis will shut down all Wallpaper and Widgets.")
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: Qt.quit()
    }
}