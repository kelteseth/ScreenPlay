#include "wallpapertimelinesection.h"
#include "ScreenPlay/screenplaywallpaper.h"

bool ScreenPlay::WallpaperTimelineSection::close(){
    for (auto& wallpaper : activeWallpaperList) {
        wallpaper->close();
    }
    activeWallpaperList.clear();
    return true;
}
