import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayApp
import ScreenPlay


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
            onNext: function (filePath) {
                root.wizardStarted();
                swipeView.currentIndex = 1;
                createWallpaperVideoImportConvert.filePath = filePath;
                App.util.setNavigationActive(false);
                App.create.createWallpaperStart(filePath);
            }
        }

        ImportWebmConvert {
            id: createWallpaperVideoImportConvert

            onExit: root.wizardExited()
        }
    }
}
