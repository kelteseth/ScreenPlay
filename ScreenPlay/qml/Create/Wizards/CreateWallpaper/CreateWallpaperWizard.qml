import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../"

Item {
    id: createNew
    anchors.fill: parent
    state: "out"

    property string filePath
    property bool canNext: false

    Component.onCompleted: {
        state = "in"
        ScreenPlay.util.setNavigationActive(false)

        wizard.setSource(
                    "qrc:/qml/Create/Wizards/CreateWallpaper/CreateWallpaperVideoImportConvert.qml",
                    {
                        "filePath": createNew.filePath
                    })
    }


    Connections {
        target: ScreenPlay.create
        onCreateWallpaperStateChanged: {
            if (state === CreateImportVideo.AnalyseVideoError || state
                    === CreateImportVideo.ConvertingVideoError || state
                    === CreateImportVideo.AnalyseVideoHasNoVideoStreamError || state
                    === CreateImportVideo.ConvertingPreviewGifError || state
                    === CreateImportVideo.ConvertingPreviewImageError || state
                    === CreateImportVideo.ConvertingAudioError || state
                    === CreateImportVideo.AbortCleanupError || state
                    === CreateImportVideo.CopyFilesError || state
                    === CreateImportVideo.CreateProjectFileError || state
                    === CreateImportVideo.CreateTmpFolderError) {
                createNew.state = "error"

                ScreenPlay.setTrackerSendEvent("createWallpaperErrorState", state.toString());
            }
        }
    }

    Timer {
        interval: 1000
        triggeredOnStart: false
        running: true
        repeat: false
        onTriggered: {
            ScreenPlay.create.createWallpaperStart(filePath)
        }
    }

    Wizard {
        id:wizard
    }

}






/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
 ##^##*/
