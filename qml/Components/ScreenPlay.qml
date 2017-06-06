import QtQuick 2.6
import QtAV 1.07


Item {
    id:empty
    anchors.fill: parent


    Loader {
        anchors.fill: parent
        source: "qrc:/qml/Components/Screens/ScreenVideo.qml"
    }

    Connections {
        target: installedListModel
        onSetScreenToVideo:{
            installedListModel.setScreenVisibleFromQml(true)
            video.stop()
            video.source = absolutePath;
            video.play();
            print(absolutePath)
        }

    }
    Video {
        id: video
        anchors.fill: parent
        implicitWidth: parent.width
        fillMode: Qt.KeepAspectRatio
    }
}
