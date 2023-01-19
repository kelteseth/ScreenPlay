// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <qobjectdefs.h>

namespace ScreenPlay {
/*!
    \namespace ScreenPlay::SearchType
    \inmodule ScreenPlayUtil
    \brief Global enum for search types. Used in the "Installed" tab.
*/
namespace SearchType {
    Q_NAMESPACE

    enum class SearchType {
        All,
        Text,
        Scene, // QML, HTML, Godot, Gif, Website wallpaper
        Wallpaper,
        Widget,
    };
    Q_ENUM_NS(SearchType)

}
/*!
    \namespace ScreenPlay::FillMode
    \inmodule ScreenPlayUtil
    \brief Global enum for fill mode. This is a c++ representation
           of HTML fill modes. We use "_" instead of "-" for scale down,
           because c++ forbids "-" in enum names.
*/
namespace FillMode {
    Q_NAMESPACE

    enum class FillMode {
        Stretch,
        Fill,
        Contain,
        Cover,
        Scale_Down
    };
    Q_ENUM_NS(FillMode)

}
/*!
    \namespace ScreenPlay::InstalledType
    \inmodule ScreenPlayUtil
    \brief When changing the enum, one also needs to change:
         GlobalVariables::getAvailableWallpaper
         GlobalVariables::getAvailableWidgets
         Common/Util.js isWallpaper() and isWidget()
         ScreenPlayWallpaper: BaseWindow::parseWallpaperType()
*/
namespace InstalledType {
    Q_NAMESPACE

    enum class InstalledType {
        Unknown,
        // Wallpaper
        VideoWallpaper,
        QMLWallpaper,
        HTMLWallpaper,
        GodotWallpaper,
        GifWallpaper,
        WebsiteWallpaper,
        // Widgets
        QMLWidget,
        HTMLWidget,
    };
    Q_ENUM_NS(InstalledType)

}

/*!
    \namespace ScreenPlay::InstalledType
    \inmodule ScreenPlayUtil
    \brief When changing the enum, one also needs to change:
         GlobalVariables::getAvailableWallpaper
         GlobalVariables::getAvailableWidgets
         Common/Util.js isWallpaper() and isWidget()
         ScreenPlayWallpaper: BaseWindow::parseWallpaperType()
*/
namespace VideoCodec {
    Q_NAMESPACE

    enum class VideoCodec {
        Unknown,
        VP8,
        VP9,
        AV1,
        H264,
        H265
    };
    Q_ENUM_NS(VideoCodec)
}
}
