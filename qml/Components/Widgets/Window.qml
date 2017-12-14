import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0


Window {
    id: mainWindow
    visible: true
    width: 640

    height: 480
    //color: "transparent"
    flags: Qt.SplashScreen | Qt.ToolTip  | Qt.WindowStaysOnBottomHint


    Rectangle {
        id: rectangle
        anchors.fill: parent;
        color: "#b31f2934"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Text {
            id: text1
            color: "#ffffff"
            text: "empty"
            font.pixelSize: 29
            onTextChanged: print("changed")
        }


    }

    MouseArea {
        property variant clickPos: "1,1"
        anchors.fill: parent;


        onPressed: {
            clickPos = Qt.point(mouse.x,mouse.y)
        }

        onPositionChanged: {
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            var new_x = mainWindow.x + delta.x
            var new_y = mainWindow.y + delta.y
            if (new_y <= 0)
                mainWindow.visibility = Window.Maximized
            else
            {
                if (mainWindow.visibility === Window.Maximized)
                    mainWindow.visibility = Window.Windowed
                mainWindow.x = new_x
                mainWindow.y = new_y
            }
        }
    }

}
