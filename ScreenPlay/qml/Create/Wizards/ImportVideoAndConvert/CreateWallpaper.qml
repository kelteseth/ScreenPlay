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

        CreateWallpaperInit {
            onNext: function (filePath, codec) {
                startConvert(filePath, codec);
            }

            function startConvert(filePath, codec) {
                root.wizardStarted();
                swipeView.currentIndex = 1;
                createWallpaperVideoImportConvert.codec = codec;
                createWallpaperVideoImportConvert.filePath = filePath;
                App.create.createWallpaperStart(filePath, codec, quality);
            }
        }

        CreateWallpaperVideoImportConvert {
            id: createWallpaperVideoImportConvert

            onAbort: root.wizardExited()
        }

        CreateWallpaperResult {
        }
    }
}
