import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

Item {
    id: root

    signal wizardStarted
    signal wizardExited
    signal next

    SwipeView {
        id: swipeView
        anchors.fill: parent
        interactive: false
        clip: true

        ImportWebmInit {
            onNext: {
                root.wizardStarted()
                swipeView.currentIndex = 1
                createWallpaperVideoImportConvert.filePath = filePath
                ScreenPlay.util.setNavigationActive(false)
                ScreenPlay.create.createWallpaperStart(filePath, Create.VP9)
            }
        }
        ImportWebmConvert {
            id: createWallpaperVideoImportConvert
            onExit: root.wizardExited()
        }
    }
}
