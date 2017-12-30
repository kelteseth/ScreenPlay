import QtQuick 2.9


Rectangle {
    id: screenVideo
    color: "gray"

    ScreenVideoPlayer {
        id: screenVideoPlayer
    }
//    Rectangle {
//        anchors.centerIn: parent
//        width:400
//        height:400
//        color: "red"
//    }
//    Component.onCompleted: wallpaper.setupWindow()
}
