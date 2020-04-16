import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id: root
    property string filePath

    signal next

    SwipeView {
        id: swipeView
        anchors.fill: parent
        interactive: false
        clip: true

        CreateWallpaperCodec {
            onNext: {
                swipeView.currentIndex = 1
                ScreenPlay.create.createWallpaperStart(filePath,codec)
            }
            onCodecChanged:createWallpaperVideoImportConvert.codec = codec
        }
        CreateWallpaperVideoImportConvert {
            id:createWallpaperVideoImportConvert
            filePath: root.filePath
        }
        CreateWallpaperResult {}
    }


    Connections {
        target: ScreenPlay.create

        function onCreateWallpaperStateChanged(state) {
            switch (state) {
            case CreateImportVideo.AnalyseVideoError:
            case CreateImportVideo.AnalyseVideoHasNoVideoStreamError:
            case CreateImportVideo.ConvertingPreviewVideoError:
            case CreateImportVideo.ConvertingPreviewGifError:
            case CreateImportVideo.ConvertingPreviewImageError:
            case CreateImportVideo.ConvertingPreviewImageThumbnailError:
            case CreateImportVideo.CopyFilesError:
            case CreateImportVideo.CreateProjectFileError:
            case CreateImportVideo.AbortCleanupError:
            case CreateImportVideo.CreateTmpFolderError:

                break
            }
        }
        function onProgressChanged(progress) {
            var percentage = Math.floor(progress * 100)

            if (percentage > 100 || progress > 0.95)
                percentage = 100

            txtConvertNumber.text = percentage + "%"
        }
    }

}
