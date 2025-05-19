import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
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
        CreateWallpaperFileSelect {
            onNext: function (filePath) {
                createWallpaperVideoImportConvert.filePath = filePath;
                swipeView.currentIndex = 1;
            }
        }

        CreateWallpaperSettings {
            id: createWallpaperSettings
            onNext: function (codec, quality) {
                root.wizardStarted();
                swipeView.currentIndex = 2;
                createWallpaperVideoImportConvert.codec = codec;
                App.create.createWallpaperStart(createWallpaperVideoImportConvert.filePath, codec, quality);
            }
        }

        CreateWallpaperVideoImportConvert {
            id: createWallpaperVideoImportConvert

            onAbort: root.wizardExited()
        }

        CreateWallpaperResult {}
    }
}
