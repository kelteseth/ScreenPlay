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

    signal wizardStarted
    signal wizardExited
    signal next

    SwipeView {
        id: swipeView
        anchors.fill: parent
        interactive: false
        clip: true

        CreateWallpaperCodec {
            onNext: {
                root.wizardStarted()
                swipeView.currentIndex = 1
                ScreenPlay.create.createWallpaperStart(filePath,codec)
            }
            onCodecChanged:createWallpaperVideoImportConvert.codec = codec
            onVideoImportConvertFileSelected: root.filePath = projectFile
        }
        CreateWallpaperVideoImportConvert {
            id:createWallpaperVideoImportConvert
            filePath: root.filePath
            onAbort:root.wizardExited()

        }
        CreateWallpaperResult {}
    }



}
