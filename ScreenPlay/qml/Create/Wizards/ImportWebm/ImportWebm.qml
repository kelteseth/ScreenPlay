import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay 1.0
import ScreenPlay.Create 1.0

Item {
    id: root

    signal wizardStarted()
    signal wizardExited()
    signal next()

    SwipeView {
        id: swipeView

        anchors.fill: parent
        interactive: false
        clip: true

        ImportWebmInit {
            onNext: (filePath) =>{
                root.wizardStarted();
                swipeView.currentIndex = 1;
                createWallpaperVideoImportConvert.filePath = filePath;
                ScreenPlay.util.setNavigationActive(false);
                ScreenPlay.create.createWallpaperStart(filePath);
            }
        }

        ImportWebmConvert {
            id: createWallpaperVideoImportConvert

            onExit: root.wizardExited()
        }

    }

}
